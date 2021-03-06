/*******************************************************************
 * Config.cpp
 *
 *  @date: 5-8-2014
 *  @author: DB
 ******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common/Config.h"
#include "common/Utils.h"
#include "common/traceout.h"
#include "builder/Templater.h"

//---------------------------------------------------------------------------------------
// Protocol constants
//---------------------------------------------------------------------------------------
const std::string Config::MAX_THREAD_NUMBER  = "MAX_THREAD_NUMBER";
const std::string Config::PORT               = "PORT";
const std::string Config::WORKING_DIR        = "WORKING_DIR";
const std::string Config::ROOT_DIR           = "ROOT_DIR";
const std::string Config::PAGE_TITLE         = "PAGE_TITLE";

//---------------------------------------------------------------------------------------
char workingDir[FILENAME_MAX]={0};
std::map<std::string, std::string> Config::settings =
{
   {MAX_THREAD_NUMBER,                       "4"},
   {PORT,                                    "8080"},
   {PAGE_TITLE,                              "HttpServer"},
   {WORKING_DIR,                             std::string(getcwd(workingDir, sizeof(workingDir)))},
   {ROOT_DIR,                                std::string(getcwd(workingDir, sizeof(workingDir)))},
};

//---------------------------------------------------------------------------------------
void Config::setValue(const std::string& valueName, const std::string& value)
//---------------------------------------------------------------------------------------
{
   settings[valueName] = value;
}

//---------------------------------------------------------------------------------------
void Config::setValue(const std::string& valueName, unsigned long value)
//---------------------------------------------------------------------------------------
{
   setValue(valueName, Utils::to_string(value));
}

//---------------------------------------------------------------------------------------
const std::string Config::getValueStr(const std::string& valueName)
//---------------------------------------------------------------------------------------
{
   std::string result = "UNKNOWN";

   auto iter = settings.find(valueName);

   if (iter != settings.end())
   {
      result = iter->second;
   }

   return result;
}

//---------------------------------------------------------------------------------------
unsigned long Config::getValueInt(const std::string& valueName)
//---------------------------------------------------------------------------------------
{
   std::string valueStr = getValueStr(valueName);
   return Utils::to_int(valueStr.c_str());
}
