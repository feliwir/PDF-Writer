/*
   Source File : OpenTypePrimitiveReader.cpp


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
#include "text/opentype/OpenTypePrimitiveReader.h"

using namespace charta;

OpenTypePrimitiveReader::OpenTypePrimitiveReader(charta::IByteReaderWithPosition *inOpenTypeFile)
{
    SetOpenTypeStream(inOpenTypeFile);
}

void OpenTypePrimitiveReader::SetOpenTypeStream(charta::IByteReaderWithPosition *inOpenTypeFile)
{
    mOpenTypeFile = inOpenTypeFile;
    if (inOpenTypeFile != nullptr)
    {
        mInitialPosition = inOpenTypeFile->GetCurrentPosition();
        mInternalState = charta::eSuccess;
    }
    else
    {
        mInternalState = charta::eFailure;
    }
}

EStatusCode OpenTypePrimitiveReader::ReadBYTE(unsigned char &outValue)
{
    if (charta::eFailure == mInternalState)
        return charta::eFailure;

    uint8_t buffer;
    EStatusCode status = (mOpenTypeFile->Read(&buffer, 1) == 1 ? charta::eSuccess : charta::eFailure);

    if (charta::eFailure == status)
        mInternalState = charta::eFailure;
    outValue = buffer;
    return status;
}

EStatusCode OpenTypePrimitiveReader::ReadCHAR(char &outValue)
{
    uint8_t buffer;

    if (ReadBYTE(buffer) != charta::eSuccess)
        return charta::eFailure;

    outValue = (char)buffer;
    return charta::eSuccess;
}

EStatusCode OpenTypePrimitiveReader::ReadUSHORT(uint16_t &outValue)
{
    uint8_t byte1, byte2;

    if (ReadBYTE(byte1) != charta::eSuccess)
        return charta::eFailure;

    if (ReadBYTE(byte2) != charta::eSuccess)
        return charta::eFailure;

    outValue = ((uint16_t)byte1 << 8) + byte2;

    return charta::eSuccess;
}

EStatusCode OpenTypePrimitiveReader::ReadSHORT(short &outValue)
{
    uint16_t buffer;

    if (ReadUSHORT(buffer) != charta::eSuccess)
        return charta::eFailure;

    outValue = (short)buffer;
    return charta::eSuccess;
}

EStatusCode OpenTypePrimitiveReader::ReadULONG(unsigned long &outValue)
{
    uint8_t byte1, byte2, byte3, byte4;

    if (ReadBYTE(byte1) != charta::eSuccess)
        return charta::eFailure;

    if (ReadBYTE(byte2) != charta::eSuccess)
        return charta::eFailure;

    if (ReadBYTE(byte3) != charta::eSuccess)
        return charta::eFailure;

    if (ReadBYTE(byte4) != charta::eSuccess)
        return charta::eFailure;

    outValue = ((unsigned long)byte1 << 24) + ((unsigned long)byte2 << 16) + ((unsigned long)byte3 << 8) + byte4;

    return charta::eSuccess;
}

EStatusCode OpenTypePrimitiveReader::ReadLONG(long &outValue)
{
    unsigned long buffer;

    if (ReadULONG(buffer) != charta::eSuccess)
        return charta::eFailure;

    outValue = (long)buffer;
    return charta::eSuccess;
}

EStatusCode OpenTypePrimitiveReader::ReadLongDateTime(long long &outValue)
{
    uint8_t byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8;

    if (ReadBYTE(byte1) != charta::eSuccess)
        return charta::eFailure;

    if (ReadBYTE(byte2) != charta::eSuccess)
        return charta::eFailure;

    if (ReadBYTE(byte3) != charta::eSuccess)
        return charta::eFailure;

    if (ReadBYTE(byte4) != charta::eSuccess)
        return charta::eFailure;

    if (ReadBYTE(byte5) != charta::eSuccess)
        return charta::eFailure;

    if (ReadBYTE(byte6) != charta::eSuccess)
        return charta::eFailure;

    if (ReadBYTE(byte7) != charta::eSuccess)
        return charta::eFailure;

    if (ReadBYTE(byte8) != charta::eSuccess)
        return charta::eFailure;

    outValue = (long long)(((unsigned long long)byte1 << 56) + ((unsigned long long)byte2 << 48) +
                           ((unsigned long long)byte3 << 40) + ((unsigned long long)byte4 << 32) +
                           ((unsigned long long)byte5 << 24) + ((unsigned long long)byte6 << 16) +
                           ((unsigned long long)byte7 << 8) + byte8);
    return charta::eSuccess;
}

void OpenTypePrimitiveReader::SetOffset(long long inNewOffset)
{
    if (mInternalState != charta::eFailure)
        mOpenTypeFile->SetPosition(mInitialPosition + inNewOffset);
}

void OpenTypePrimitiveReader::Skip(size_t inToSkip)
{
    if (mInternalState != charta::eFailure)
        mOpenTypeFile->Skip(inToSkip);
}

EStatusCode OpenTypePrimitiveReader::GetInternalState()
{
    return mInternalState;
}

EStatusCode OpenTypePrimitiveReader::ReadFixed(double &outValue)
{
    uint16_t integer, fraction;

    if (ReadUSHORT(integer) != charta::eSuccess)
        return charta::eFailure;

    if (ReadUSHORT(fraction) != charta::eSuccess)
        return charta::eFailure;

    outValue = (double)integer + ((double)fraction) / (1 << 16);

    return charta::eSuccess;
}

long long OpenTypePrimitiveReader::GetCurrentPosition()
{
    if (mInternalState != charta::eFailure)
        return mOpenTypeFile->GetCurrentPosition() - mInitialPosition;
    return 0;
}

EStatusCode OpenTypePrimitiveReader::Read(uint8_t *inBuffer, size_t inBufferSize)
{
    if (charta::eFailure == mInternalState)
        return charta::eFailure;

    EStatusCode status =
        (mOpenTypeFile->Read(inBuffer, inBufferSize) == inBufferSize ? charta::eSuccess : charta::eFailure);

    if (charta::eFailure == status)
        mInternalState = charta::eFailure;
    return status;
}

charta::IByteReaderWithPosition *OpenTypePrimitiveReader::GetReadStream()
{
    return mOpenTypeFile;
}