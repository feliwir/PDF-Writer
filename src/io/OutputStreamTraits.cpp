/*
   Source File : OutputStreamTraits.cpp


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
#include "io/OutputStreamTraits.h"
#include "io/IByteReaderWithPosition.h"
#include "io/IByteWriter.h"

using namespace charta;

OutputStreamTraits::OutputStreamTraits(IByteWriter *inOutputStream)
{
    mOutputStream = inOutputStream;
}

#define TENMEGS 10 * 1024 * 1024
EStatusCode OutputStreamTraits::CopyToOutputStream(IByteReader *inInputStream)
{
    auto *buffer = new uint8_t[TENMEGS];
    size_t readBytes, writeBytes;
    EStatusCode status = charta::eSuccess;

    while (inInputStream->NotEnded() && charta::eSuccess == status)
    {
        readBytes = inInputStream->Read(buffer, TENMEGS);
        writeBytes = mOutputStream->Write(buffer, readBytes);
        status = (readBytes == writeBytes) ? charta::eSuccess : charta::eFailure;
        if (readBytes == 0)
        {
            break; // for whatever reason notEnded is not reached...dont want this to interfere
        }
    }
    delete[] buffer;
    return status;
}

EStatusCode OutputStreamTraits::CopyToOutputStream(IByteReader *inInputStream, size_t inLength)
{
    auto *buffer = new uint8_t[inLength];
    size_t readBytes, writeBytes;

    readBytes = inInputStream->Read(buffer, inLength);
    writeBytes = mOutputStream->Write(buffer, readBytes);
    EStatusCode status = (readBytes == writeBytes) ? charta::eSuccess : charta::eFailure;
    delete[] buffer;
    return status;
}