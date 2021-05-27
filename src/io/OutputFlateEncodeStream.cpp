/*
   Source File : OutputFlateEncodeStream.cpp


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
#include "io/OutputFlateEncodeStream.h"
#include "Trace.h"
#include <zlib.h>

constexpr size_t BUFFER_SIZE = 256 * 1024;

charta::OutputFlateEncodeStream::OutputFlateEncodeStream()
{
    mBuffer = new uint8_t[BUFFER_SIZE];
    mZLibState = new z_stream;
    mTargetStream = nullptr;
    mCurrentlyEncoding = false;
}

charta::OutputFlateEncodeStream::~OutputFlateEncodeStream()
{
    if (mCurrentlyEncoding)
        FinalizeEncoding();
    if (mTargetStream != nullptr)
        delete mTargetStream;
    delete[] mBuffer;
    delete mZLibState;
}

void charta::OutputFlateEncodeStream::FinalizeEncoding()
{
    // flush leftovers by repeatedly calling with Z_FINISH parameter
    int deflateResult;

    mZLibState->avail_in = 0;
    mZLibState->next_in = nullptr;

    do
    {
        mZLibState->avail_out = BUFFER_SIZE;
        mZLibState->next_out = mBuffer;
        deflateResult = deflate(mZLibState, Z_FINISH);
        if (Z_STREAM_ERROR == deflateResult)
        {
            TRACE_LOG1("charta::OutputFlateEncodeStream::FinalizeEncoding, failed to flush zlib information. returned "
                       "error code = %d",
                       deflateResult);
            break;
        }

        size_t writtenBytes;
        writtenBytes = mTargetStream->Write(mBuffer, BUFFER_SIZE - mZLibState->avail_out);
        if (writtenBytes != BUFFER_SIZE - mZLibState->avail_out)
        {
            TRACE_LOG2("charta::OutputFlateEncodeStream::FinalizeEncoding, Failed to write the desired amount of zlib "
                       "bytes to underlying stream. supposed to write %lld, wrote %lld",
                       BUFFER_SIZE - mZLibState->avail_out, writtenBytes);
            break;
        }

    } while (Z_OK == deflateResult); // waiting for either an error, or Z_STREAM_END
    deflateEnd(mZLibState);
    mCurrentlyEncoding = false;
}

charta::OutputFlateEncodeStream::OutputFlateEncodeStream(charta::IByteWriterWithPosition *inTargetWriter,
                                                         bool inInitiallyOn)
{
    mBuffer = new uint8_t[BUFFER_SIZE];
    mZLibState = new z_stream;
    mTargetStream = nullptr;
    mCurrentlyEncoding = false;

    Assign(inTargetWriter, inInitiallyOn);
}

void charta::OutputFlateEncodeStream::StartEncoding()
{
    mZLibState->zalloc = Z_NULL;
    mZLibState->zfree = Z_NULL;
    mZLibState->opaque = Z_NULL;

    int deflateStatus = deflateInit(mZLibState, Z_DEFAULT_COMPRESSION);
    if (deflateStatus != Z_OK)
        TRACE_LOG1("charta::OutputFlateEncodeStream::StartEncoding, Unexpected failure in initializating flate "
                   "library. status "
                   "code = %d",
                   deflateStatus);
    else
        mCurrentlyEncoding = true;
}

void charta::OutputFlateEncodeStream::Assign(charta::IByteWriterWithPosition *inWriter, bool inInitiallyOn)
{
    if (mCurrentlyEncoding)
        FinalizeEncoding();
    mTargetStream = inWriter;
    if (inInitiallyOn && (mTargetStream != nullptr))
        StartEncoding();
}

size_t charta::OutputFlateEncodeStream::Write(const uint8_t *inBuffer, size_t inSize)
{
    if (mCurrentlyEncoding)
        return EncodeBufferAndWrite(inBuffer, inSize);
    if (mTargetStream != nullptr)
        return mTargetStream->Write(inBuffer, inSize);
    return 0;
}

size_t charta::OutputFlateEncodeStream::EncodeBufferAndWrite(const uint8_t *inBuffer, size_t inSize)
{
    int deflateResult;

    mZLibState->avail_in = (uInt)inSize; // hmm, caveat here...should take care of this sometime.
    mZLibState->next_in = (Bytef *)inBuffer;

    do
    {
        mZLibState->avail_out = BUFFER_SIZE;
        mZLibState->next_out = mBuffer;
        deflateResult = deflate(mZLibState, Z_NO_FLUSH);
        if (Z_STREAM_ERROR == deflateResult)
        {
            TRACE_LOG1(
                "charta::OutputFlateEncodeStream::EncodeBufferAndWrite, failed to write zlib information. returned "
                "error code = %d",
                deflateResult);
            break;
        }

        size_t writtenBytes;
        writtenBytes = mTargetStream->Write(mBuffer, BUFFER_SIZE - mZLibState->avail_out);
        if (writtenBytes != BUFFER_SIZE - mZLibState->avail_out)
        {
            TRACE_LOG2(
                "charta::OutputFlateEncodeStream::EncodeBufferAndWrite, Failed to write the desired amount of zlib "
                "bytes to underlying stream. supposed to write %lld, wrote %lld",
                BUFFER_SIZE - mZLibState->avail_out, writtenBytes);
            deflateEnd(mZLibState);
            deflateResult = Z_STREAM_ERROR;
            mCurrentlyEncoding = false;
            break;
        }

    } while (mZLibState->avail_out == 0); // waiting for either no more writes

    if (Z_OK == deflateResult)
        return inSize;
    return 0;
}

long long charta::OutputFlateEncodeStream::GetCurrentPosition()
{
    if (mTargetStream != nullptr)
        return mTargetStream->GetCurrentPosition();
    return 0;
}

void charta::OutputFlateEncodeStream::TurnOnEncoding()
{
    if (!mCurrentlyEncoding)
        StartEncoding();
}

void charta::OutputFlateEncodeStream::TurnOffEncoding()
{
    if (mCurrentlyEncoding)
        FinalizeEncoding();
}
