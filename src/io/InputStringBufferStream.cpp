/*
   Source File : InputStringBufferStream.cpp


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
#include "io/InputStringBufferStream.h"
#include "SafeBufferMacrosDefs.h"

InputStringBufferStream::InputStringBufferStream(MyStringBuf *inBufferToReadFrom)
{
    mBufferToReadFrom = inBufferToReadFrom;
}

void InputStringBufferStream::Assign(MyStringBuf *inBufferToReadFrom)
{
    mBufferToReadFrom = inBufferToReadFrom;
}

InputStringBufferStream::~InputStringBufferStream() = default;

size_t InputStringBufferStream::Read(uint8_t *inBuffer, size_t inBufferSize)
{
    return (size_t)mBufferToReadFrom->SAFE_SGETN((char *)inBuffer, inBufferSize, inBufferSize);
}

bool InputStringBufferStream::NotEnded()
{
    return mBufferToReadFrom->in_avail() != 0;
}

void InputStringBufferStream::Skip(size_t inSkipSize)
{
    mBufferToReadFrom->pubseekoff(inSkipSize, std::ios_base::cur);
}

void InputStringBufferStream::SetPosition(long long inOffsetFromStart)
{
    mBufferToReadFrom->pubseekoff((long)inOffsetFromStart, std::ios_base::beg);
}

void InputStringBufferStream::SetPositionFromEnd(long long inOffsetFromEnd)
{
    mBufferToReadFrom->pubseekoff((long)inOffsetFromEnd, std::ios_base::end);
}

long long InputStringBufferStream::GetCurrentPosition()
{
    return mBufferToReadFrom->GetCurrentReadPosition();
}