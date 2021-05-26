/*
   Source File : InputBufferedStream.cpp


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
#include "io/InputBufferedStream.h"
#include <algorithm>
#include <memory.h>

InputBufferedStream::InputBufferedStream()
{
    Initiate(nullptr, DEFAULT_INPUT_BUFFER_SIZE);
}

InputBufferedStream::InputBufferedStream(size_t inBufferSize)
{
    Initiate(nullptr, inBufferSize);
}

InputBufferedStream::InputBufferedStream(std::unique_ptr<IByteReaderWithPosition> inSourceReader, size_t inBufferSize)
{
    Initiate(std::move(inSourceReader), inBufferSize);
}

void InputBufferedStream::Assign(std::unique_ptr<IByteReaderWithPosition> inReader)
{
    mSourceStream = std::move(inReader);
    mCurrentBufferIndex = 0;
}

size_t InputBufferedStream::Read(uint8_t *inBuffer, size_t inBufferSize)
{
    if (mSourceStream != nullptr)
    {
        size_t bytesRead;

        // if there are enough bytes to read from the buffer..just read from the buffer
        // if content to write fits in the buffer write to buffer
        if (inBufferSize <= mLastAvailableIndex - mCurrentBufferIndex)
        {
            if (inBufferSize > 0)
            {
                std::copy(mBuffer.data() + mCurrentBufferIndex, mBuffer.data() + mCurrentBufferIndex + inBufferSize,
                          inBuffer);
                mCurrentBufferIndex += inBufferSize;
            }
            bytesRead = inBufferSize;
        }
        else
        {
            // if not, read what's left in the buffer, then read moduluso of buffer size directly to the output buffer
            // then read some leftovers to the buffer, and read from the buffer to the output

            // read what's currently in the buffer into the output buffer
            std::copy(mBuffer.data() + mCurrentBufferIndex,
                      mBuffer.data() + mCurrentBufferIndex + (mLastAvailableIndex - mCurrentBufferIndex), inBuffer);
            bytesRead = mLastAvailableIndex - mCurrentBufferIndex;
            mCurrentBufferIndex = mLastAvailableIndex;

            // if still need to read more than mBufferSize, read all but modulo of buffer size directly to the output
            // buffer [so that can later read into buffer, and copy from there to the output buffer

            if (mSourceStream->NotEnded())
            {
                inBufferSize -= bytesRead;
                // We can't read more than the internal buffersize
                size_t bytesToReadToBuffer = inBufferSize % mBuffer.size();

                // Read what we can read now
                bytesRead += mSourceStream->Read(inBuffer + bytesRead, inBufferSize - bytesToReadToBuffer);

                if (mSourceStream->NotEnded())
                {
                    mLastAvailableIndex = mSourceStream->Read(mBuffer.data(), mBuffer.size());
                    mCurrentBufferIndex = 0;
                    bytesToReadToBuffer = std::min<size_t>(bytesToReadToBuffer, mLastAvailableIndex);
                    if (bytesToReadToBuffer > 0)
                    {
                        std::copy(mBuffer.data() + mCurrentBufferIndex,
                                  mBuffer.data() + mCurrentBufferIndex + bytesToReadToBuffer, inBuffer + bytesRead);
                        mCurrentBufferIndex += bytesToReadToBuffer;
                        bytesRead += bytesToReadToBuffer;
                    }
                }
            }
        }
        return bytesRead;
    }
    return 0;
}

bool InputBufferedStream::NotEnded()
{
    return mSourceStream->NotEnded() || (mCurrentBufferIndex != mLastAvailableIndex);
}

void InputBufferedStream::Initiate(std::unique_ptr<IByteReaderWithPosition> inSourceReader, size_t inBufferSize)
{
    mBuffer.resize(inBufferSize);
    mLastAvailableIndex = mCurrentBufferIndex = 0;
    mSourceStream = std::move(inSourceReader);
}

void InputBufferedStream::Skip(size_t inSkipSize)
{
    if (inSkipSize <= mLastAvailableIndex - mCurrentBufferIndex)
    {
        mCurrentBufferIndex += inSkipSize;
    }
    else
    {
        inSkipSize -= mLastAvailableIndex - mCurrentBufferIndex;
        mCurrentBufferIndex = mLastAvailableIndex;
        mSourceStream->Skip(inSkipSize);
    }
}

void InputBufferedStream::SetPosition(long long inOffsetFromStart)
{
    mLastAvailableIndex = mCurrentBufferIndex = 0;
    mSourceStream->SetPosition(inOffsetFromStart);
}

void InputBufferedStream::SetPositionFromEnd(long long inOffsetFromEnd)
{
    mLastAvailableIndex = mCurrentBufferIndex = 0;
    mSourceStream->SetPositionFromEnd(inOffsetFromEnd);
}

IByteReaderWithPosition *InputBufferedStream::GetSourceStream()
{
    return mSourceStream.get();
}

long long InputBufferedStream::GetCurrentPosition()
{
    // when reading the current position is the current stream position minus how much is left
    // to read from the buffer
    return mSourceStream->GetCurrentPosition() - (mLastAvailableIndex - mCurrentBufferIndex);
}