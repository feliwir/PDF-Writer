/*
Source File : InputStringStream.cpp


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
#include "io/InputStringStream.h"

charta::InputStringStream::InputStringStream(const std::string &inString)
{
    mStartPosition = inString.begin();
    mEndPosition = inString.end();
    mCurrentPosition = mStartPosition;
}

charta::InputStringStream::~InputStringStream() = default;

void charta::InputStringStream::Assign(const std::string &inString)
{
    mStartPosition = inString.begin();
    mEndPosition = inString.end();
    mCurrentPosition = mStartPosition;
}

size_t charta::InputStringStream::Read(uint8_t *inBuffer, size_t inBufferSize)
{
    size_t amountRead = 0;

    for (; amountRead < inBufferSize && mCurrentPosition != mEndPosition; ++amountRead, ++mCurrentPosition)
        inBuffer[amountRead] = (uint8_t)*mCurrentPosition;
    return amountRead;
}

bool charta::InputStringStream::NotEnded()
{
    return mCurrentPosition != mEndPosition;
}

void charta::InputStringStream::Skip(size_t inSkipSize)
{
    mCurrentPosition =
        (inSkipSize > (size_t)(mEndPosition - mCurrentPosition)) ? mEndPosition : (mCurrentPosition + inSkipSize);
    ;
}

void charta::InputStringStream::SetPosition(long long inOffsetFromStart)
{
    mCurrentPosition = inOffsetFromStart > (long long)(mEndPosition - mStartPosition)
                           ? mEndPosition
                           : (mStartPosition + (std::string::size_type)inOffsetFromStart);
}

void charta::InputStringStream::SetPositionFromEnd(long long inOffsetFromEnd)
{
    mCurrentPosition =
        inOffsetFromEnd > (long long)(mEndPosition - mStartPosition)
            ? mStartPosition
            : (mStartPosition + (mEndPosition - mStartPosition - (std::string::size_type)inOffsetFromEnd));
}

long long charta::InputStringStream::GetCurrentPosition()
{
    return mCurrentPosition - mStartPosition;
}
