/*
   Source File : InputPredictorTIFFSubStream.cpp


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
#include "io/InputPredictorTIFFSubStream.h"
#include "Trace.h"

charta::InputPredictorTIFFSubStream::InputPredictorTIFFSubStream()
{
    mSourceStream = nullptr;
    mRowBuffer = nullptr;
    mReadColors = nullptr;
    mReadColorsCount = 0;
}

charta::InputPredictorTIFFSubStream::InputPredictorTIFFSubStream(charta::IByteReader *inSourceStream, size_t inColors,
                                                                 uint8_t inBitsPerComponent, size_t inColumns)
{
    mSourceStream = nullptr;
    mRowBuffer = nullptr;
    mReadColors = nullptr;

    Assign(inSourceStream, inColors, inBitsPerComponent, inColumns);
}

charta::InputPredictorTIFFSubStream::~InputPredictorTIFFSubStream()
{
    delete mSourceStream;
    delete[] mRowBuffer;
    delete[] mReadColors;
}

size_t charta::InputPredictorTIFFSubStream::Read(uint8_t *inBuffer, size_t inBufferSize)
{
    size_t readBytes = 0;

    // exhaust what's in the buffer currently
    while (mReadColorsCount > (size_t)(mReadColorsIndex - mReadColors) && readBytes < inBufferSize)
    {
        ReadByteFromColorsArray(inBuffer[readBytes]);
        ++readBytes;
    }

    // now repeatedly read bytes from the input stream, and decode
    while (readBytes < inBufferSize && mSourceStream->NotEnded())
    {
        if (mSourceStream->Read(mRowBuffer, (mColumns * mColors * mBitsPerComponent) / 8) !=
            (mColumns * mColors * mBitsPerComponent) / 8)
        {
            TRACE_LOG("InputPredictorPNGSubStream::Read, problem, expected columns*colors*bitspercomponent/8 number "
                      "read. didn't make it");
            readBytes = 0;
            break;
        }
        DecodeBufferToColors();

        while (mReadColorsCount > (size_t)(mReadColorsIndex - mReadColors) && readBytes < inBufferSize)
        {
            ReadByteFromColorsArray(inBuffer[readBytes]);
            ++readBytes;
        }
    }
    return readBytes;
}

bool charta::InputPredictorTIFFSubStream::NotEnded()
{
    return mSourceStream->NotEnded() || (size_t)(mReadColorsIndex - mReadColors) < mReadColorsCount;
}

void charta::InputPredictorTIFFSubStream::Assign(charta::IByteReader *inSourceStream, size_t inColors,
                                                 uint8_t inBitsPerComponent, size_t inColumns)
{
    mSourceStream = inSourceStream;
    mColors = inColors;
    mBitsPerComponent = inBitsPerComponent;
    mColumns = inColumns;

    delete mRowBuffer;
    mRowBuffer = new uint8_t[(inColumns * inColors * inBitsPerComponent) / 8];

    mReadColorsCount = inColumns * inColors;
    mReadColors = new uint16_t[mReadColorsCount];
    mReadColorsIndex =
        mReadColors + mReadColorsCount; // assign to end of array so will know that should read new buffer
    mIndexInColor = 0;

    mBitMask = 0;
    for (uint8_t i = 0; i < inBitsPerComponent; ++i)
        mBitMask = (mBitMask << 1) + 1;
}

void charta::InputPredictorTIFFSubStream::ReadByteFromColorsArray(uint8_t &outBuffer)
{
    if (8 == mBitsPerComponent)
    {
        outBuffer = (uint8_t)(*mReadColorsIndex);
        ++mReadColorsIndex;
    }
    else if (8 > mBitsPerComponent)
    {
        outBuffer = 0;
        for (uint8_t i = 0; i < (8 / mBitsPerComponent); ++i)
        {
            outBuffer = (outBuffer << mBitsPerComponent) + (uint8_t)(*mReadColorsIndex);
            ++mReadColorsIndex;
        }
    }
    else // 8 < mBitsPerComponent [which is just 16 for now]
    {
        outBuffer = (uint8_t)(((*mReadColorsIndex) >> (mBitsPerComponent - mIndexInColor * 8)) & 0xff);
        ++mIndexInColor;
        if (mBitsPerComponent / 8 == mIndexInColor)
        {
            ++mReadColorsIndex;
            mIndexInColor = 0;
        }
    }
}

void charta::InputPredictorTIFFSubStream::DecodeBufferToColors()
{
    // 1. Split to colors. Use array of colors (should be columns * colors). Each time take BitsPerComponent of the
    // buffer
    // 2. Once you got the "colors", loop the array, setting values after diffs (use modulo of bit mask for "sign"
    // computing)
    // 3. Now you have the colors array.
    size_t i = 0;

    // read the colors differences according to bits per component
    if (8 == mBitsPerComponent)
    {
        for (; i < mReadColorsCount; ++i)
            mReadColors[i] = mRowBuffer[i];
    }
    else if (8 > mBitsPerComponent)
    {
        for (; i < (mReadColorsCount * mBitsPerComponent / 8); ++i)
        {
            for (size_t j = 0; j < (size_t)(8 / mBitsPerComponent); ++j)
            {
                mReadColors[(i + 1) * 8 / mBitsPerComponent - j - 1] = mRowBuffer[i] & mBitMask;
                mRowBuffer[i] = mRowBuffer[i] >> mBitsPerComponent;
            }
        }
    }
    else // mBitesPerComponent > 8
    {
        for (; i < mReadColorsCount; ++i)
        {
            mReadColors[i] = 0;
            for (uint8_t j = 0; j < mBitsPerComponent / 8; ++j)
                mReadColors[i] = (mReadColors[i] << mBitsPerComponent) + mRowBuffer[i * mBitsPerComponent / 8 + j];
        }
    }

    // calculate color values according to diffs
    for (i = mColors; i < mReadColorsCount; ++i)
        mReadColors[i] = (mReadColors[i] + mReadColors[i - mColors]) & mBitMask;

    mReadColorsIndex = mReadColors;
    mIndexInColor = 0;
}