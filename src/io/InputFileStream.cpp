/*
   Source File : InputFileStream.cpp


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
#include "io/InputFileStream.h"
#include "SafeBufferMacrosDefs.h"

using namespace PDFHummus;

InputFileStream::~InputFileStream()
{
    if (mStream != nullptr)
        Close();
}

InputFileStream::InputFileStream(const std::string &inFilePath)
{
    Open(inFilePath);
}

EStatusCode InputFileStream::Open(const std::string &inFilePath)
{
    SAFE_FOPEN(mStream, inFilePath.c_str(), "rb");
    return nullptr == mStream ? PDFHummus::eFailure : PDFHummus::eSuccess;
}

EStatusCode InputFileStream::Close()
{
    EStatusCode result = fclose(mStream) == 0 ? PDFHummus::eSuccess : PDFHummus::eFailure;

    mStream = nullptr;
    return result;
}

size_t InputFileStream::Read(uint8_t *inBuffer, size_t inBufferSize)
{
    size_t readItems = mStream != nullptr ? fread(static_cast<void *>(inBuffer), 1, inBufferSize, mStream) : 0;
    return readItems;
}

bool InputFileStream::NotEnded()
{
    if (mStream != nullptr)
        return feof(mStream) == 0;
    return false;
}

void InputFileStream::Skip(size_t inSkipSize)
{
    if (mStream != nullptr)
        SAFE_FSEEK64(mStream, inSkipSize, SEEK_CUR);
}

void InputFileStream::SetPosition(long long inOffsetFromStart)
{
    if (mStream != nullptr)
        SAFE_FSEEK64(mStream, inOffsetFromStart, SEEK_SET);
}

long long InputFileStream::GetCurrentPosition()
{
    if (mStream != nullptr)
        return SAFE_FTELL64(mStream);
    return 0;
}

long long InputFileStream::GetFileSize()
{
    if (mStream != nullptr)
    {
        // very messy...prefer a different means sometime
        long long currentPosition = SAFE_FTELL64(mStream);
        long long result;

        SAFE_FSEEK64(mStream, 0, SEEK_END);
        result = SAFE_FTELL64(mStream);
        SAFE_FSEEK64(mStream, currentPosition, SEEK_SET);
        return result;
    }
    return 0;
}

void InputFileStream::SetPositionFromEnd(long long inOffsetFromEnd)
{
    if (mStream != nullptr)
    {
        // if failed, probaby means that seeks too much, so place at file begin.
        if (SAFE_FSEEK64(mStream, -inOffsetFromEnd, SEEK_END) != 0)
            SAFE_FSEEK64(mStream, 0, SEEK_SET);
    }
}
