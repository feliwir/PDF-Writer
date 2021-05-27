/*
   Source File : OutputFileStream.cpp


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
#include "io/OutputFileStream.h"
#include "SafeBufferMacrosDefs.h"

charta::OutputFileStream::~OutputFileStream()
{
    Close();
}

charta::OutputFileStream::OutputFileStream(const std::string &inFilePath, bool inAppend)
{
    Open(inFilePath, inAppend);
}

charta::EStatusCode charta::OutputFileStream::Open(const std::string &inFilePath, bool inAppend)
{
    // seek to end, so position reading gets the correct file position, even before first write
    mStream.open(inFilePath, std::ios::binary | std::ios::ate | (inAppend ? std::ios::app : std::ios::trunc));

    if (mStream.fail())
        return charta::eFailure;

    return charta::eSuccess;
};

charta::EStatusCode charta::OutputFileStream::Close()
{
    mStream.close();
    EStatusCode result = mStream.fail() ? charta::eFailure : charta::eSuccess;

    return result;
}

size_t charta::OutputFileStream::Write(const uint8_t *inBuffer, size_t inSize)
{
    if (!mStream.is_open())
        return 0;

    auto before = mStream.tellp();
    mStream.write((const char *)inBuffer, inSize);
    return mStream.tellp() - before;
}

long long charta::OutputFileStream::GetCurrentPosition()
{
    if (!mStream.is_open())
        return 0;

    return mStream.tellp();
}