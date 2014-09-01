#include "stdio.h"
#include <exception>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>

#include "core/Dispatcher.h"
#include "core/Connection.h"
#include "common/traceout.h"
#include "common/Utils.h"
#include "common/Config.h"
#include "common/File.h"
#include "builder/Templater.h"

#include <TestHarness.h>
#include <thread>

#include <bits/unique_ptr.h>
#include <bits/shared_ptr.h>


//---------------------------------------------------------------------------------------
// FORWARD DECLARATIONS
//---------------------------------------------------------------------------------------
bool parseOptions(int argc, char *argv[]);
void printUsage();
void daemonize();
bool checkEnv();


//---------------------------------------------------------------------------------------
int main(int argc, char *argv[])
//---------------------------------------------------------------------------------------
{
   TRC_DEBUG_FUNC_ENTER(0U, "Application started");

   if (!parseOptions(argc, argv))
   {
      printUsage();
      exit(EXIT_FAILURE);
   }

   if (!checkEnv())
   {
      printUsage();
      exit(EXIT_FAILURE);
   }

   TRC_INFO(0U, "Deamonization");
   ////daemonize();

   /* Open the log file */
   TRC_INIT(LOG_PID, LOG_DAEMON);

   try
   {
      HTTP::Dispatcher dispatcher;
      HTTP::Connection connection;
      connection.setPort(Config::getValueInt(Config::PORT));

      // initialize
      dispatcher.setConnection(connection);

      TRC_INFO(0U, "Server is being started...");

      // blocking call: starts infinite message loop
      dispatcher.start();
   }

   catch (const std::exception& e)
   {
      TRC_FATAL(0U, "Fatal error occured: '%s'", e.what());
      TRC_FATAL(0U, "Application terminated!");
   }

   TRC_DEINIT();

   return 0;
}

//---------------------------------------------------------------------------------------
bool checkEnv()
//---------------------------------------------------------------------------------------
{
   bool bResult = true;

   std::string workDir = Config::getValueStr(Config::WORKING_DIR);

   bResult &= File(workDir + Templater::PATH_ROOT_LAYOUT)        .exists();
   bResult &= File(workDir + Templater::PATH_DIR_CONTENT)        .exists();
   bResult &= File(workDir + Templater::PATH_DIR_CONTENT_LINE)   .exists();
   bResult &= File(workDir + Templater::PATH_STR_CONTENT)        .exists();
   bResult &= File(workDir + Templater::PATH_STR_CONTENT_LINE)   .exists();

   return bResult;
}

//---------------------------------------------------------------------------------------
void reloadTemplates()
//---------------------------------------------------------------------------------------
{
   Templater::purgeCache();

   return;
}

//---------------------------------------------------------------------------------------
void signalWaitProc()
//---------------------------------------------------------------------------------------
{
   struct sigaction sigact;
   sigset_t         sigset;
   int             signo;
   int             status;

   // ������� �� ������� � ��������� ����� ������������ ����� ���������
   // ��������� ��� ����� �������� ����������� ���������� �� �������
   sigact.sa_flags = SA_SIGINFO;

//   // ������ ������� ���������� ��������
//   sigact.sa_sigaction = signal_error;

   sigemptyset(&sigact.sa_mask);

   // ��������� ��� ���������� �� �������

   sigaction(SIGFPE, &sigact, 0); // ������ FPU
   sigaction(SIGILL, &sigact, 0); // ��������� ����������
   sigaction(SIGSEGV, &sigact, 0); // ������ ������� � ������
   sigaction(SIGBUS, &sigact, 0); // ������ ����, ��� ��������� � ���������� ������

   sigemptyset(&sigset);

   // ��������� ������� ������� ����� �������
   // ������ ��������� �������� �������������
   sigaddset(&sigset, SIGQUIT);

   // ������ ��� ��������� �������� ������������� � ���������
   sigaddset(&sigset, SIGINT);

   // ������ ������� ���������� ��������
   sigaddset(&sigset, SIGTERM);

   // ���������������� ������ ������� �� ����� ������������ ��� ���������� �������
   sigaddset(&sigset, SIGUSR1);
   sigprocmask(SIG_BLOCK, &sigset, NULL);

   if (!status)
   {
       // ���� �������� ���������
       for (;;)
       {
           // ���� ��������� ���������
           sigwait(&sigset, &signo);

           // ���� �� ��������� ���������� �������
           if (signo == SIGUSR1)
           {
               // ������� ������
               ////status = ReloadConfig();
               reloadTemplates();
               if (status == 0)
               {
                   ///WriteLog("[DAEMON] Reload config failed\n");
               }
               else
               {
                   ///WriteLog("[DAEMON] Reload config OK\n");
               }
           }
           else // ���� �����-���� ������ ������, �� ������ �� �����
           {
               break;
           }
       }
   }

   return;
}

//---------------------------------------------------------------------------------------
void daemonize()
//---------------------------------------------------------------------------------------
{
   pid_t pid;

   /* Fork off the parent process */
   pid = fork();

   /* An error occurred */
   if (pid < 0)
      exit(EXIT_FAILURE);

   /* Success: Let the parent terminate */
   if (pid > 0)
      exit(EXIT_SUCCESS);

   /* On success: The child process becomes session leader */
   if (setsid() < 0)
      exit(EXIT_FAILURE);

   /* Catch, ignore and handle signals */
   //TODO: Implement a working signal handler */
   signal(SIGCHLD, SIG_IGN );
   signal(SIGHUP, SIG_IGN );

   /* Fork off for the second time*/
   pid = fork();

   /* An error occurred */
   if (pid < 0)
      exit(EXIT_FAILURE);

   /* Success: Let the parent terminate */
   if (pid > 0)
      exit(EXIT_SUCCESS);

   /* Set new file permissions */
   umask(0);

   /* Set new session to stay independent from parent */
   setsid();

   /* Change the working directory to the root directory */
   /* or another appropriated directory */
   std::string workDir = Config::getValueStr(Config::WORKING_DIR);
   chdir(workDir.c_str());

   /* Close all open file descriptors */
   int fileDescrs;
   for (fileDescrs = sysconf(_SC_OPEN_MAX); fileDescrs > 0; fileDescrs--)
   {
      close(fileDescrs);
   }

   std::thread(signalWaitProc);
}

//---------------------------------------------------------------------------------------
bool parseOptions(int argc, char *argv[])
//---------------------------------------------------------------------------------------
{
   bool bResult = true;
   int c;

   opterr = 0;

   while ((c = getopt(argc, argv, "p:d:t:r:")) != -1)
   {
      switch (c)
      {
      case 'p':
         Config::setValue(Config::PORT, Utils::to_int(optarg));
         break;

      case 'd':
         Config::setValue(Config::WORKING_DIR, std::string(optarg));
         break;

      case 'r':
         Config::setValue(Config::ROOT_DIR, std::string(optarg));
         break;

      case 't':
         Config::setValue(Config::MAX_THREAD_NUMBER, Utils::to_int(optarg));
         break;

      case '?':
      default:
         fprintf(stdout, "Unknown/invalid arguments.\n\n");
         bResult = false;
         break;
      }
   }

   return bResult;
}

//---------------------------------------------------------------------------------------
void printUsage()
//---------------------------------------------------------------------------------------
{
   fprintf(stdout,
         "Usage: HttpServer [-p <port>] [-d <work_dir>] [-r <root_dir>] [-t <max_num_of_threads>] \n\
\n\
port               - port number to listen for client connection. By default: 8080\n\
work_dir           - working server directory. MUST(!) contain 'templates' folder. By default: current dir.\n\
root_dir           - root directory to start the server in. By default: current dir.\n\
max_num_of_threads - maximum number of threads. By default: 4\n\
\n");
}
