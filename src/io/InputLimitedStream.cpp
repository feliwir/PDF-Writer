/*
   Source File : InputLimitedStream.cpp


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
#include "io/InputLimitedStream.h"

#include <algorithm>

InputLimitedStream::InputLimitedStream()
{
    mStream = nullptr;
    mMoreToRead = 0;
    mOwnsStream = false;
}

InputLimitedStream::~InputLimitedStream()
{
    if ((mStream != nullptr) && mOwnsStream)
        delete mStream;
}

InputLimitedStream::InputLimitedStream(IByteReader *inSourceStream, long long inReadLimit, bool inOwnsStream)
{
    Assign(inSourceStream, inReadLimit, inOwnsStream);
}

void InputLimitedStream::Assign(IByteReader *inSourceStream, long long inReadLimit, bool inOwnsStream)
{
    mStream = inSourceStream;
    mMoreToRead = inReadLimit;
    mOwnsStream = inOwnsStream;
}

size_t InputLimitedStream::Read(uint8_t *inBuffer, size_t inBufferSize)
{
    size_t readBytes = mStream->Read(inBuffer, (size_t)std::min<long long>((long long)inBufferSize, mMoreToRead));

    mMoreToRead -= readBytes;
    return readBytes;
}

bool InputLimitedStream::NotEnded()
{
    return mStream->NotEnded() && mMoreToRead > 0;
}
