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

charta::OutputBufferedStream::OutputBufferedStream()
{
    Initiate(nullptr, DEFAULT_OUTPUT_BUFFER_SIZE);
}

void charta::OutputBufferedStream::Initiate(std::unique_ptr<IByteWriterWithPosition> inTargetWriter,
                                            size_t inBufferSize)
{
    mBuffer.resize(inBufferSize);
    mCurrentBufferIndex = 0;
    mTargetStream = std::move(inTargetWriter);
}

charta::OutputBufferedStream::~OutputBufferedStream()
{
    Flush();
}

charta::OutputBufferedStream::OutputBufferedStream(size_t inBufferSize)
{
    Initiate(nullptr, inBufferSize);
}

charta::OutputBufferedStream::OutputBufferedStream(std::unique_ptr<IByteWriterWithPosition> inTargetWriter,
                                                   size_t inBufferSize)
{
    Initiate(std::move(inTargetWriter), inBufferSize);
}

void charta::OutputBufferedStream::Assign(std::unique_ptr<IByteWriterWithPosition> inWriter)
{
    Flush();
    mTargetStream = std::move(inWriter);
}

size_t charta::OutputBufferedStream::Write(const uint8_t *inBuffer, size_t inSize)
{
    if (mTargetStream != nullptr)
    {
        size_t bytesWritten;

        // if content to write fits in the buffer write to buffer
        if (inSize <= mBuffer.size() - mCurrentBufferIndex)
        {
            if (inSize > 0)
            {
                std::copy(inBuffer, inBuffer + inSize, mBuffer.begin() + mCurrentBufferIndex);
                mCurrentBufferIndex += inSize;
            }
            bytesWritten = inSize;
        }
        else
        {
            // if not, flush the buffer. if now won't fit in the buffer write directly to underlying stream
            // all but what size will fit in the buffer - then write to buffer what leftover will fit in.
            size_t bytesToWriteToBuffer = inSize % mBuffer.size();
            Flush();

            bytesWritten = mTargetStream->Write(inBuffer, inSize - bytesToWriteToBuffer);
            if ((inSize - bytesToWriteToBuffer == bytesWritten) && bytesToWriteToBuffer > 0) // all well, continue
            {
                std::copy(inBuffer + (inSize - bytesToWriteToBuffer),
                          inBuffer + (inSize - bytesToWriteToBuffer) + bytesToWriteToBuffer,
                          mBuffer.begin() + mCurrentBufferIndex);
                mCurrentBufferIndex += bytesToWriteToBuffer;
                bytesWritten += bytesToWriteToBuffer;
            }
        }
        return bytesWritten;
    }
    return 0;
}

void charta::OutputBufferedStream::Flush()
{
    if ((mTargetStream != nullptr) && mCurrentBufferIndex != 0)
        mTargetStream->Write(mBuffer.data(), mCurrentBufferIndex);
    mCurrentBufferIndex = 0;
}

long long charta::OutputBufferedStream::GetCurrentPosition()
{
    return mTargetStream != nullptr ? mTargetStream->GetCurrentPosition() + mCurrentBufferIndex : 0;
}

charta::IByteWriterWithPosition *charta::OutputBufferedStream::GetTargetStream()
{
    return mTargetStream.get();
}