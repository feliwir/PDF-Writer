/*
   Source File : OutputBufferedStream.cpp


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
#include "io/OutputBufferedStream.h"

#include <memory.h>

OutputBufferedStream::OutputBufferedStream()
{
    Initiate(nullptr, DEFAULT_BUFFER_SIZE);
}

void OutputBufferedStream::Initiate(IByteWriterWithPosition *inTargetWriter, size_t inBufferSize)
{
    mBufferSize = inBufferSize;
    mBuffer = new uint8_t[mBufferSize];
    mCurrentBufferIndex = mBuffer;
    mTargetStream = inTargetWriter;
}

OutputBufferedStream::~OutputBufferedStream()
{
    Flush();
    delete[] mBuffer;
    delete mTargetStream;
}

OutputBufferedStream::OutputBufferedStream(size_t inBufferSize)
{
    Initiate(nullptr, inBufferSize);
}

OutputBufferedStream::OutputBufferedStream(IByteWriterWithPosition *inTargetWriter, size_t inBufferSize)
{
    Initiate(inTargetWriter, inBufferSize);
}

void OutputBufferedStream::Assign(IByteWriterWithPosition *inWriter)
{
    Flush();
    mTargetStream = inWriter;
}

size_t OutputBufferedStream::Write(const uint8_t *inBuffer, size_t inSize)
{
    if (mTargetStream != nullptr)
    {
        size_t bytesWritten;

        // if content to write fits in the buffer write to buffer
        if (inSize <= mBufferSize - (mCurrentBufferIndex - mBuffer))
        {
            if (inSize > 0)
            {
                memcpy(mCurrentBufferIndex, inBuffer, inSize);
                mCurrentBufferIndex += inSize;
            }
            bytesWritten = inSize;
        }
        else
        {
            // if not, flush the buffer. if now won't fit in the buffer write directly to underlying stream
            // all but what size will fit in the buffer - then write to buffer what leftover will fit in.
            size_t bytesToWriteToBuffer = inSize % mBufferSize;
            Flush();

            bytesWritten = mTargetStream->Write(inBuffer, inSize - bytesToWriteToBuffer);
            if ((inSize - bytesToWriteToBuffer == bytesWritten) && bytesToWriteToBuffer > 0) // all well, continue
            {
                memcpy(mCurrentBufferIndex, inBuffer + (inSize - bytesToWriteToBuffer), bytesToWriteToBuffer);
                mCurrentBufferIndex += bytesToWriteToBuffer;
                bytesWritten += bytesToWriteToBuffer;
            }
        }
        return bytesWritten;
    }
    return 0;
}

void OutputBufferedStream::Flush()
{
    if ((mTargetStream != nullptr) && mCurrentBufferIndex != mBuffer)
        mTargetStream->Write(mBuffer, mCurrentBufferIndex - mBuffer);
    mCurrentBufferIndex = mBuffer;
}

long long OutputBufferedStream::GetCurrentPosition()
{
    return mTargetStream != nullptr ? mTargetStream->GetCurrentPosition() + (mCurrentBufferIndex - mBuffer) : 0;
}