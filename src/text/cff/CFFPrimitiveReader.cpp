/*
   Source File : CFFPrimitiveReader.cpp


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
#include "CFFPrimitiveReader.h"
#include <math.h>

using namespace PDFHummus;

CFFPrimitiveReader::CFFPrimitiveReader(IByteReaderWithPosition *inCFFFile)
{
    SetStream(inCFFFile);
}

void CFFPrimitiveReader::SetStream(IByteReaderWithPosition *inCFFFile)
{
    mCFFFile = inCFFFile;
    if (inCFFFile != nullptr)
    {
        mCurrentOffsize = 1;
        mInitialPosition = inCFFFile->GetCurrentPosition();
        mInternalState = PDFHummus::eSuccess;
    }
    else
    {
        mInternalState = PDFHummus::eFailure;
    }
}

void CFFPrimitiveReader::SetOffset(long long inNewOffset)
{
    if (mInternalState != PDFHummus::eFailure)
        mCFFFile->SetPosition(mInitialPosition + inNewOffset);
}

void CFFPrimitiveReader::Skip(size_t inToSkip)
{
    if (mInternalState != PDFHummus::eFailure)
        mCFFFile->Skip(inToSkip);
}

EStatusCode CFFPrimitiveReader::GetInternalState()
{
    return mInternalState;
}

long long CFFPrimitiveReader::GetCurrentPosition()
{
    if (mInternalState != PDFHummus::eFailure)
        return mCFFFile->GetCurrentPosition() - mInitialPosition;
    return 0;
}

EStatusCode CFFPrimitiveReader::ReadByte(uint8_t &outValue)
{
    if (PDFHummus::eFailure == mInternalState)
        return PDFHummus::eFailure;

    uint8_t buffer;
    EStatusCode status = (mCFFFile->Read(&buffer, 1) == 1 ? PDFHummus::eSuccess : PDFHummus::eFailure);

    if (PDFHummus::eFailure == status)
        mInternalState = PDFHummus::eFailure;
    outValue = buffer;
    return status;
}

EStatusCode CFFPrimitiveReader::Read(uint8_t *ioBuffer, size_t inBufferSize)
{
    if (PDFHummus::eFailure == mInternalState)
        return PDFHummus::eFailure;

    EStatusCode status =
        (mCFFFile->Read(ioBuffer, inBufferSize) == inBufferSize ? PDFHummus::eSuccess : PDFHummus::eFailure);

    if (PDFHummus::eFailure == status)
        mInternalState = PDFHummus::eFailure;
    return status;
}

EStatusCode CFFPrimitiveReader::ReadCard8(uint8_t &outValue)
{
    return ReadByte(outValue);
}

EStatusCode CFFPrimitiveReader::ReadCard16(uint16_t &outValue)
{
    uint8_t byte1, byte2;

    if (ReadByte(byte1) != PDFHummus::eSuccess)
        return PDFHummus::eFailure;

    if (ReadByte(byte2) != PDFHummus::eSuccess)
        return PDFHummus::eFailure;

    outValue = ((uint16_t)byte1 << 8) + byte2;

    return PDFHummus::eSuccess;
}
EStatusCode CFFPrimitiveReader::Read2ByteSigned(short &outValue)
{
    uint16_t buffer;
    EStatusCode status = ReadCard16(buffer);

    if (status != PDFHummus::eSuccess)
        return PDFHummus::eFailure;

    outValue = (short)buffer;
    return PDFHummus::eSuccess;
}

void CFFPrimitiveReader::SetOffSize(uint8_t inOffSize)
{
    mCurrentOffsize = inOffSize;
}

EStatusCode CFFPrimitiveReader::ReadOffset(unsigned long &outValue)
{
    EStatusCode status = PDFHummus::eFailure;

    switch (mCurrentOffsize)
    {
    case 1:
        uint8_t byteBuffer;
        status = ReadCard8(byteBuffer);
        if (PDFHummus::eSuccess == status)
            outValue = byteBuffer;
        break;
    case 2:
        uint16_t shortBuffer;
        status = ReadCard16(shortBuffer);
        if (PDFHummus::eSuccess == status)
            outValue = shortBuffer;
        break;
    case 3:
        status = Read3ByteUnsigned(outValue);
        break;
    case 4:
        status = Read4ByteUnsigned(outValue);
        break;
    }

    return status;
}

EStatusCode CFFPrimitiveReader::Read3ByteUnsigned(unsigned long &outValue)
{
    uint8_t byte1, byte2, byte3;

    if (ReadByte(byte1) != PDFHummus::eSuccess)
        return PDFHummus::eFailure;

    if (ReadByte(byte2) != PDFHummus::eSuccess)
        return PDFHummus::eFailure;

    if (ReadByte(byte3) != PDFHummus::eSuccess)
        return PDFHummus::eFailure;

    outValue = ((unsigned long)byte1 << 16) + ((unsigned long)byte2 << 8) + byte3;

    return PDFHummus::eSuccess;
}

EStatusCode CFFPrimitiveReader::Read4ByteUnsigned(unsigned long &outValue)
{
    uint8_t byte1, byte2, byte3, byte4;

    if (ReadByte(byte1) != PDFHummus::eSuccess)
        return PDFHummus::eFailure;

    if (ReadByte(byte2) != PDFHummus::eSuccess)
        return PDFHummus::eFailure;

    if (ReadByte(byte3) != PDFHummus::eSuccess)
        return PDFHummus::eFailure;

    if (ReadByte(byte4) != PDFHummus::eSuccess)
        return PDFHummus::eFailure;

    outValue = ((unsigned long)byte1 << 24) + ((unsigned long)byte2 << 16) + ((unsigned long)byte3 << 8) + byte4;

    return PDFHummus::eSuccess;
}

EStatusCode CFFPrimitiveReader::Read4ByteSigned(long &outValue)
{
    unsigned long buffer;
    EStatusCode status = Read4ByteUnsigned(buffer);

    if (status != PDFHummus::eSuccess)
        return PDFHummus::eFailure;

    outValue = (int)buffer; // very important to cast to 32, to get the sign right

    return PDFHummus::eSuccess;
}

EStatusCode CFFPrimitiveReader::ReadOffSize(uint8_t &outValue)
{
    return ReadCard8(outValue);
}

EStatusCode CFFPrimitiveReader::ReadSID(uint16_t &outValue)
{
    return ReadCard16(outValue);
}

EStatusCode CFFPrimitiveReader::ReadIntegerOperand(uint8_t inFirstByte, long &outValue)
{
    uint8_t byte0, byte1;
    EStatusCode status = PDFHummus::eSuccess;

    byte0 = inFirstByte;

    if (byte0 >= 32 && byte0 <= 246)
    {
        outValue = (long)byte0 - 139;
    }
    else if (byte0 >= 247 && byte0 <= 250)
    {
        if (ReadByte(byte1) != PDFHummus::eSuccess)
            return PDFHummus::eFailure;

        outValue = (byte0 - 247) * 256 + byte1 + 108;
    }
    else if (byte0 >= 251 && byte0 <= 254)
    {
        if (ReadByte(byte1) != PDFHummus::eSuccess)
            return PDFHummus::eFailure;

        outValue = -(long)((long)byte0 - 251) * 256 - byte1 - 108;
    }
    else if (28 == byte0)
    {
        short buffer = 0;
        status = Read2ByteSigned(buffer);
        outValue = buffer;
    }
    else if (29 == byte0)
    {
        status = Read4ByteSigned(outValue);
    }
    else
        status = PDFHummus::eFailure;

    return status;
}

EStatusCode CFFPrimitiveReader::ReadRealOperand(double &outValue, long &outRealValueFractalEnd)
{
    double integerPart = 0;
    double fractionPart = 0;
    double powerPart = 0;
    double result;
    bool hasNegative = false;
    bool hasFraction = false;
    bool hasPositivePower = false;
    bool hasNegativePower = false;
    bool notDone = true;
    double fractionDecimal = 1;
    outRealValueFractalEnd = 0;
    uint8_t buffer;
    uint8_t nibble[2];
    EStatusCode status = PDFHummus::eSuccess;

    do
    {
        status = ReadByte(buffer);
        if (status != PDFHummus::eSuccess)
            break;

        nibble[0] = (buffer >> 4) & 0xf;
        nibble[1] = buffer & 0xf;

        for (unsigned char i : nibble)
        {
            switch (i)
            {
            case 0xa:
                hasFraction = true;
                break;
            case 0xb:
                hasPositivePower = true;
                break;
            case 0xc:
                hasNegativePower = true;
                break;
            case 0xd:
                // reserved
                break;
            case 0xe:
                hasNegative = true;
                break;
            case 0xf:
                notDone = false;
                break;
            default: // numbers
                if (hasPositivePower || hasNegativePower)
                {
                    powerPart = powerPart * 10 + i;
                }
                else if (hasFraction)
                {
                    fractionPart = fractionPart * 10 + i;
                    fractionDecimal *= 10;
                    ++outRealValueFractalEnd;
                }
                else
                    integerPart = integerPart * 10 + i;
            }
        }
    } while (notDone);

    if (PDFHummus::eSuccess == status)
    {
        result = integerPart + fractionPart / fractionDecimal;
        if (hasNegativePower || hasPositivePower)
            result = result * pow(10, hasNegativePower ? -powerPart : powerPart);
        if (hasNegative)
            result = -1 * result;
        outValue = result;
    }
    return status;
}

bool CFFPrimitiveReader::IsDictOperator(uint8_t inCandidate)
{
    return (inCandidate <= 27 || 31 == inCandidate);
}

EStatusCode CFFPrimitiveReader::ReadDictOperator(uint8_t inFirstByte, uint16_t &outOperator)
{
    if (12 == inFirstByte)
    {
        uint8_t buffer;
        if (ReadByte(buffer) == PDFHummus::eSuccess)
        {
            outOperator = ((uint16_t)inFirstByte << 8) | buffer;
            return PDFHummus::eSuccess;
        }
        return PDFHummus::eFailure;
    }

    outOperator = inFirstByte;
    return PDFHummus::eSuccess;
}

EStatusCode CFFPrimitiveReader::ReadDictOperand(uint8_t inFirstByte, DictOperand &outOperand)
{
    if (30 == inFirstByte) // real
    {
        outOperand.IsInteger = false;
        return ReadRealOperand(outOperand.RealValue, outOperand.RealValueFractalEnd);
    }
    if (28 == inFirstByte || 29 == inFirstByte || (32 <= inFirstByte && inFirstByte <= 246) ||
        (247 <= inFirstByte && inFirstByte <= 250) || (251 <= inFirstByte && inFirstByte <= 254))
    {
        outOperand.IsInteger = true;
        return ReadIntegerOperand(inFirstByte, outOperand.IntegerValue);
    }
    return PDFHummus::eFailure; // not an operand
}
