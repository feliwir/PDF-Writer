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
#include "InputBufferedStream.h"
#include <algorithm>
#include <memory.h>

InputBufferedStream::InputBufferedStream()
{
    Initiate(nullptr, DEFAULT_BUFFER_SIZE);
}

InputBufferedStream::~InputBufferedStream()
{
    delete[] mBuffer;
    delete mSourceStream;
}

InputBufferedStream::InputBufferedStream(size_t inBufferSize)
{
    Initiate(nullptr, inBufferSize);
}

InputBufferedStream::InputBufferedStream(IByteReaderWithPosition *inSourceReader, size_t inBufferSize)
{
    Initiate(inSourceReader, inBufferSize);
}

void InputBufferedStream::Assign(IByteReaderWithPosition *inReader)
{
    mSourceStream = inReader;
    mCurrentBufferIndex = mBuffer;
}

size_t InputBufferedStream::Read(uint8_t *inBuffer, size_t inBufferSize)
{
    if (mSourceStream != nullptr)
    {
        size_t bytesRead;

        // if there are enough bytes to read from the buffer..just read from the buffer
        // if content to write fits in the buffer write to buffer
        if (inBufferSize <= (size_t)(mLastAvailableIndex - mCurrentBufferIndex))
        {
            if (inBufferSize > 0)
            {
                memcpy(inBuffer, mCurrentBufferIndex, inBufferSize);
                mCurrentBufferIndex += inBufferSize;
            }
            bytesRead = inBufferSize;
        }
        else
        {
            // if not, read what's left in the buffer, then read moduluso of buffer size directly to the output buffer
            // then read some leftovers to the buffer, and read from the buffer to the output

            // read what's currently in the buffer into the output buffer
            memcpy(inBuffer, mCurrentBufferIndex, mLastAvailableIndex - mCurrentBufferIndex);
            bytesRead = mLastAvailableIndex - mCurrentBufferIndex;
            mCurrentBufferIndex = mLastAvailableIndex;

            // if still need to read more than mBufferSize, read all but modulo of buffer size directly to the output
            // buffer [so that can later read into buffer, and copy from there to the output buffer

            if (mSourceStream->NotEnded())
            {
                inBufferSize -= bytesRead;
                size_t bytesToReadToBuffer = inBufferSize % mBufferSize;

                bytesRead += mSourceStream->Read(inBuffer + bytesRead, inBufferSize - bytesToReadToBuffer);

                if (mSourceStream->NotEnded())
                {
                    mLastAvailableIndex = mBuffer + mSourceStream->Read(mBuffer, mBufferSize);
                    mCurrentBufferIndex = mBuffer;
                    bytesToReadToBuffer = std::min<size_t>(bytesToReadToBuffer, mLastAvailableIndex - mBuffer);
                    if (bytesToReadToBuffer > 0)
                    {
                        memcpy(inBuffer + bytesRead, mCurrentBufferIndex, bytesToReadToBuffer);
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

void InputBufferedStream::Initiate(IByteReaderWithPosition *inSourceReader, size_t inBufferSize)
{
    mBufferSize = inBufferSize;
    mBuffer = new uint8_t[mBufferSize];
    mLastAvailableIndex = mCurrentBufferIndex = mBuffer;
    mSourceStream = inSourceReader;
}

void InputBufferedStream::Skip(size_t inSkipSize)
{
    if (inSkipSize <= (size_t)(mLastAvailableIndex - mCurrentBufferIndex))
    {
        mCurrentBufferIndex += inSkipSize;
    }
    else
    {
        inSkipSize -= (size_t)(mLastAvailableIndex - mCurrentBufferIndex);
        mCurrentBufferIndex = mLastAvailableIndex;
        mSourceStream->Skip(inSkipSize);
    }
}

void InputBufferedStream::SetPosition(long long inOffsetFromStart)
{
    mLastAvailableIndex = mCurrentBufferIndex = mBuffer;
    mSourceStream->SetPosition(inOffsetFromStart);
}

void InputBufferedStream::SetPositionFromEnd(long long inOffsetFromEnd)
{
    mLastAvailableIndex = mCurrentBufferIndex = mBuffer;
    mSourceStream->SetPositionFromEnd(inOffsetFromEnd);
}

IByteReaderWithPosition *InputBufferedStream::GetSourceStream()
{
    return mSourceStream;
}

long long InputBufferedStream::GetCurrentPosition()
{
    // when reading the current position is the current stream position minus how much is left
    // to read from the buffer
    return mSourceStream->GetCurrentPosition() - (mLastAvailableIndex - mCurrentBufferIndex);
}