/*
   Source File : Log.h


   Copyright 2011 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.


*/
#pragma once

#include "io/OutputFile.h"
#include <stdint.h>
#include <stdio.h>

#include <string>

class Log;

typedef void (*LogFileMethod)(Log *inThis, const uint8_t *inMessage, size_t inMessageSize);

class IByteWriter;

class Log
{
  public:
    // log writes are in UTF8. so i'm asking here if you want a bom
    Log(const std::string &inLogFilePath, bool inPlaceUTF8Bom);
    Log(IByteWriter *inLogStream);
    ~Log(void);

    void LogEntry(const std::string &inMessage);
    void LogEntry(const uint8_t *inMessage, size_t inMessageSize);

    // don't use
    void LogEntryToFile(const uint8_t *inMessage, size_t inMessageSize);
    void LogEntryToStream(const uint8_t *inMessage, size_t inMessageSize);

  private:
    std::string mFilePath;
    OutputFile mLogFile;
    IByteWriter *mLogStream;
    LogFileMethod mLogMethod;

    std::string GetFormattedTimeString();
    void WriteLogEntryToStream(const uint8_t *inMessage, size_t inMessageSize, IByteWriter *inStream);
};
