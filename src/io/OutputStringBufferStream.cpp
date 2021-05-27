/*
   Source File : OutputStringBufferStream.cpp


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
#include "io/OutputStringBufferStream.h"

charta::OutputStringBufferStream::OutputStringBufferStream()
{
    mBuffer = new MyStringBuf();
    mOwnsBuffer = true;
}

charta::OutputStringBufferStream::~OutputStringBufferStream()
{
    if (mOwnsBuffer)
        delete mBuffer;
}

charta::OutputStringBufferStream::OutputStringBufferStream(MyStringBuf *inControlledBuffer)
{
    mBuffer = inControlledBuffer;
    mOwnsBuffer = false;
}

void charta::OutputStringBufferStream::Assign(MyStringBuf *inControlledBuffer)
{
    if (inControlledBuffer != nullptr)
    {
        if (mOwnsBuffer)
            delete mBuffer;
        mBuffer = inControlledBuffer;
        mOwnsBuffer = false;
    }
    else // passing null will cause the stream to get to a state of self writing (so as to never leave this stream in a
         // vulnarable position)s
    {
        mBuffer = new MyStringBuf();
        mOwnsBuffer = true;
    }
}

size_t charta::OutputStringBufferStream::Write(const uint8_t *inBuffer, size_t inSize)
{
    return (size_t)mBuffer->sputn((const char *)inBuffer, inSize);
}

long long charta::OutputStringBufferStream::GetCurrentPosition()
{
    return mBuffer->GetCurrentWritePosition();
}

std::string charta::OutputStringBufferStream::ToString() const
{
    return mBuffer->str();
}

static const std::string scEmpty;

void charta::OutputStringBufferStream::Reset()
{
    mBuffer->str(scEmpty);
}

void charta::OutputStringBufferStream::SetPosition(long long inOffsetFromStart)
{
    mBuffer->pubseekoff((long)inOffsetFromStart, std::ios_base::beg);
}