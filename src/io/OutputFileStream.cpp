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

using namespace PDFHummus;

OutputFileStream::~OutputFileStream()
{
    Close();
}

OutputFileStream::OutputFileStream(const std::string &inFilePath, bool inAppend)
{
    Open(inFilePath, inAppend);
}

EStatusCode OutputFileStream::Open(const std::string &inFilePath, bool inAppend)
{
    // seek to end, so position reading gets the correct file position, even before first write
    mStream.open(inFilePath, std::ios::binary | std::ios::ate | (inAppend ? std::ios::app : std::ios::trunc));

    if (mStream.fail())
        return PDFHummus::eFailure;

    return PDFHummus::eSuccess;
};

EStatusCode OutputFileStream::Close()
{
    mStream.close();
    EStatusCode result = mStream.fail() ? PDFHummus::eFailure : PDFHummus::eSuccess;

    return result;
}

size_t OutputFileStream::Write(const uint8_t *inBuffer, size_t inSize)
{
    if (!mStream.is_open())
        return 0;

    auto before = mStream.tellp();
    mStream.write((const char *)inBuffer, inSize);
    return mStream.tellp() - before;
}

long long OutputFileStream::GetCurrentPosition()
{
    if (!mStream.is_open())
        return 0;

    return mStream.tellp();
}