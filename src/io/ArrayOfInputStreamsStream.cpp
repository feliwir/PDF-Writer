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
#include "io/ArrayOfInputStreamsStream.h"
#include "objects/PDFArray.h"
#include "objects/PDFObjectCast.h"
#include "objects/PDFStreamInput.h"
#include "parsing/PDFParser.h"

ArrayOfInputStreamsStream::ArrayOfInputStreamsStream(std::shared_ptr<PDFArray> inArrayOfStreams, PDFParser *inParser)
{
    mArray = inArrayOfStreams;
    mParser = inParser;
    mCurrentStream = nullptr;
    mCurrentIndex = 0;
}

ArrayOfInputStreamsStream::~ArrayOfInputStreamsStream()
{
    delete mCurrentStream;
}

IByteReader *ArrayOfInputStreamsStream::GetActiveStream()
{

    if ((mCurrentStream != nullptr) && mCurrentStream->NotEnded())
        return mCurrentStream;

    // delete current stream, done with it
    if (mCurrentStream != nullptr)
    {
        ++mCurrentIndex;
    }
    delete mCurrentStream;
    mCurrentStream = nullptr;

    // get next stream in array
    PDFObjectCastPtr<PDFStreamInput> aStream;

    while ((mCurrentStream == nullptr) && mCurrentIndex < mArray->GetLength())
    {
        PDFObjectCastPtr<PDFStreamInput> aStream = mParser->QueryArrayObject(mArray, mCurrentIndex);
        if (!!aStream)
        {
            mCurrentStream = mParser->StartReadingFromStream(aStream);
            // couldn't start, try with next array object
            if (mCurrentStream == nullptr)
                ++mCurrentIndex;
        }
        else
        {
            // Not stream, try again with next array object
            ++mCurrentIndex;
        }
    }

    return mCurrentStream;
}

size_t ArrayOfInputStreamsStream::Read(uint8_t *inBuffer, size_t inBufferSize)
{
    if (!NotEnded())
        return 0;

    size_t readAmount = 0;

    while (NotEnded() && readAmount < inBufferSize)
    {
        // read from current stream
        IByteReader *reader = GetActiveStream();
        if ((reader != nullptr) && reader->NotEnded())
        {
            readAmount += reader->Read(inBuffer + readAmount, inBufferSize - readAmount);
        }
    }
    return readAmount;
}

bool ArrayOfInputStreamsStream::NotEnded()
{
    return !!mArray && mCurrentIndex < mArray->GetLength();
}