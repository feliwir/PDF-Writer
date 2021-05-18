/*
   Source File : InputByteArrayStream.cpp


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
#include "InputByteArrayStream.h"
#include <memory.h>

InputByteArrayStream::InputByteArrayStream()
{
    mByteArray = nullptr;
}

InputByteArrayStream::InputByteArrayStream(uint8_t *inByteArray, long long inArrayLength)
{
    mByteArray = inByteArray;
    mArrayLength = inArrayLength;
    mCurrentPosition = 0;
}

InputByteArrayStream::~InputByteArrayStream() = default;

void InputByteArrayStream::Assign(uint8_t *inByteArray, long long inArrayLength)
{
    mByteArray = inByteArray;
    mArrayLength = inArrayLength;
    mCurrentPosition = 0;
}

size_t InputByteArrayStream::Read(uint8_t *inBuffer, size_t inBufferSize)
{
    if (mByteArray == nullptr)
        return 0;

    size_t amountToRead = inBufferSize < (size_t)(mArrayLength - mCurrentPosition)
                              ? inBufferSize
                              : (size_t)(mArrayLength - mCurrentPosition);

    if (amountToRead > 0)
        memcpy(inBuffer, mByteArray + mCurrentPosition, amountToRead);
    mCurrentPosition += amountToRead;
    return amountToRead;
}

bool InputByteArrayStream::NotEnded()
{
    return (mByteArray != nullptr) && mCurrentPosition < mArrayLength;
}

void InputByteArrayStream::Skip(size_t inSkipSize)
{
    mCurrentPosition +=
        inSkipSize < (size_t)mArrayLength - mCurrentPosition ? inSkipSize : mArrayLength - mCurrentPosition;
}

void InputByteArrayStream::SetPosition(long long inOffsetFromStart)
{
    mCurrentPosition = inOffsetFromStart > mArrayLength ? mArrayLength : inOffsetFromStart;
}

void InputByteArrayStream::SetPositionFromEnd(long long inOffsetFromEnd)
{
    mCurrentPosition = inOffsetFromEnd > mArrayLength ? 0 : (mArrayLength - inOffsetFromEnd);
}

long long InputByteArrayStream::GetCurrentPosition()
{
    return mCurrentPosition;
}
