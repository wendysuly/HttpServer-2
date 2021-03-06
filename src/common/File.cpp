/*******************************************************************
 * File.cpp
 *
 *  @date: 5-8-2014
 *  @author: DB
 ******************************************************************/

#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <exception>
#include <stdio.h>
#include <fstream>

#include "common/File.h"
#include "common/traceout.h"
#include "common/Config.h"


//---------------------------------------------------------------------------------------
File::File() :
  mFilePath(""),
  mName(""),
  mIsDir(false),
  mSize(-1)
//---------------------------------------------------------------------------------------
{
   TRC_DEBUG_FUNC_ENTER(0U, "");
   TRC_DEBUG_FUNC_EXIT (0U);
}

//---------------------------------------------------------------------------------------
File::File(const std::string& filePath) :
  mFilePath(filePath),
  mName(filePath),//getBaseName(filePath)),
  mIsDir(false),
  mSize(-1)
//---------------------------------------------------------------------------------------
{
}

//---------------------------------------------------------------------------------------
File::~File()
//---------------------------------------------------------------------------------------
{
   TRC_DEBUG_FUNC_ENTER(0U, "");
   TRC_DEBUG_FUNC_EXIT (0U);
}

//---------------------------------------------------------------------------------------
bool File::exists()
//---------------------------------------------------------------------------------------
{
   return (!mFilePath.empty() && access( mFilePath.c_str(), F_OK ) != -1);
}

//---------------------------------------------------------------------------------------
std::string File::getPermissions() const
//---------------------------------------------------------------------------------------
{
   if (0 == mPermissions.size())
   {
      loadFileMetadata();
   }
   return mPermissions;
}

//---------------------------------------------------------------------------------------
unsigned long File::getSize() const
//---------------------------------------------------------------------------------------
{
   if (-1 == mSize)
   {
      loadFileMetadata();
   }
   return mSize;
}

//---------------------------------------------------------------------------------------
const std::string& File::getName() const
//---------------------------------------------------------------------------------------
{
   return mName;
}

//---------------------------------------------------------------------------------------
std::string File::getReference() const
//---------------------------------------------------------------------------------------
{
   return "\"" + mReferenceFilePath + (isDir() ? "/" : "") + "\"";
}

//---------------------------------------------------------------------------------------
void File::setName(const std::string& name)
//---------------------------------------------------------------------------------------
{
   mName = name;
}

//---------------------------------------------------------------------------------------
void File::loadFileMetadata() const
//---------------------------------------------------------------------------------------
{
   struct stat statInfo;

   if (stat(mFilePath.c_str(), &statInfo) == -1)
   {
      throw(std::ios_base::failure(std::string("Failed to load metadata for the file '") + mFilePath + ": " + strerror(errno)));
   }

   char perms[1 + 3 + 3 + 3];
   sprintf( perms,
            "%c%c%c%c%c%c%c%c%c%c",

            S_ISDIR(statInfo.st_mode) ? 'd' : '-',

            statInfo.st_mode && S_IRUSR ? 'r' : '-', statInfo.st_mode && S_IWUSR ? 'w' : '-',
            statInfo.st_mode && S_IXUSR ? 'x' : '-',

            statInfo.st_mode && S_IRGRP ? 'r' : '-', statInfo.st_mode && S_IWGRP ? 'w' : '-',
            statInfo.st_mode && S_IXGRP ? 'x' : '-',

            statInfo.st_mode && S_IROTH ? 'r' : '-', statInfo.st_mode && S_IWOTH ? 'w' : '-',
            statInfo.st_mode && S_IXOTH ? 'x' : '-'
          );

   mSize          = statInfo.st_size;
   mPermissions   = perms;
}

//---------------------------------------------------------------------------------------
bool File::isDir() const
//---------------------------------------------------------------------------------------
{
   return mIsDir;
}

//---------------------------------------------------------------------------------------
void File::setDir(bool isDir)
//---------------------------------------------------------------------------------------
{
   mIsDir = isDir;
}
