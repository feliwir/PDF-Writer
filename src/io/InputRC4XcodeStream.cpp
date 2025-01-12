/*
    Source File : InputRC4XcodeStream.h


    Copyright 2016 Gal Kahana PDFWriter

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

#include "io/InputRC4XcodeStream.h"

charta::InputRC4XcodeStream::InputRC4XcodeStream()
{
    mSourceStream = nullptr;
}

charta::InputRC4XcodeStream::InputRC4XcodeStream(charta::IByteReader *inSourceReader, const ByteList &inKey)
    : mRC4(inKey)
{
    mSourceStream = inSourceReader;
}

charta::InputRC4XcodeStream::~InputRC4XcodeStream()
{
    if (mSourceStream != nullptr)
        delete mSourceStream;
}

void charta::InputRC4XcodeStream::Assign(charta::IByteReader *inSourceReader, const ByteList &inKey)
{
    mSourceStream = inSourceReader;
    mRC4.Reset(inKey);
}

bool charta::InputRC4XcodeStream::NotEnded()
{
    return (mSourceStream != nullptr) && mSourceStream->NotEnded();
}

size_t charta::InputRC4XcodeStream::Read(uint8_t *inBuffer, size_t inBufferSize)
{
    size_t mCurrentIndex = 0;
    uint8_t buffer;

    while (NotEnded() && mCurrentIndex < inBufferSize)
    {
        mSourceStream->Read(&buffer, 1);
        inBuffer[mCurrentIndex] = mRC4.DecodeNextByte(buffer);
        ++mCurrentIndex;
    }

    return mCurrentIndex;
}