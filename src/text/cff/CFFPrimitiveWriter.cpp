/*
 Source File : CFFPrimitiveWriter.cpp


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
#include "text/cff/CFFPrimitiveWriter.h"
#include <math.h>

using namespace charta;

CFFPrimitiveWriter::CFFPrimitiveWriter(charta::IByteWriter *inCFFOutput)
{
    SetStream(inCFFOutput);
}

void CFFPrimitiveWriter::SetStream(charta::IByteWriter *inCFFOutput)
{
    mCFFOutput = inCFFOutput;
    if (inCFFOutput != nullptr)
    {
        mCurrentOffsize = 1;
        mInternalState = charta::eSuccess;
    }
    else
    {
        mInternalState = charta::eFailure;
    }
}

EStatusCode CFFPrimitiveWriter::GetInternalState()
{
    return mInternalState;
}

EStatusCode CFFPrimitiveWriter::WriteByte(uint8_t inValue)
{
    if (charta::eFailure == mInternalState)
        return charta::eFailure;

    EStatusCode status = (mCFFOutput->Write(&inValue, 1) == 1 ? charta::eSuccess : charta::eFailure);

    if (charta::eFailure == status)
        mInternalState = charta::eFailure;
    return status;
}

EStatusCode CFFPrimitiveWriter::Write(const uint8_t *inBuffer, size_t inBufferSize)
{
    if (charta::eFailure == mInternalState)
        return charta::eFailure;

    EStatusCode status =
        (mCFFOutput->Write(inBuffer, inBufferSize) == inBufferSize ? charta::eSuccess : charta::eFailure);

    if (charta::eFailure == status)
        mInternalState = charta::eFailure;
    return status;
}

EStatusCode CFFPrimitiveWriter::WriteCard8(uint8_t inValue)
{
    return WriteByte(inValue);
}

EStatusCode CFFPrimitiveWriter::WriteCard16(uint16_t inValue)
{
    uint8_t byte1 = (inValue >> 8) & 0xff;
    uint8_t byte2 = inValue & 0xff;

    if (WriteByte(byte1) != charta::eSuccess)
        return charta::eFailure;

    if (WriteByte(byte2) != charta::eSuccess)
        return charta::eFailure;

    return charta::eSuccess;
}

void CFFPrimitiveWriter::SetOffSize(uint8_t inOffSize)
{
    mCurrentOffsize = inOffSize;
}

EStatusCode CFFPrimitiveWriter::WriteOffset(unsigned long inValue)
{
    EStatusCode status = charta::eFailure;

    switch (mCurrentOffsize)
    {
    case 1:
        status = WriteCard8((uint8_t)inValue);
        break;
    case 2:
        status = WriteCard16((uint16_t)inValue);
        break;
    case 3:
        status = Write3ByteUnsigned(inValue);
        break;
    case 4:
        status = Write4ByteUnsigned(inValue);
        break;
    }
    return status;
}

EStatusCode CFFPrimitiveWriter::Write3ByteUnsigned(unsigned long inValue)
{
    uint8_t byte1 = (inValue >> 16) & 0xff;
    uint8_t byte2 = (inValue >> 8) & 0xff;
    uint8_t byte3 = inValue & 0xff;

    if (WriteByte(byte1) != charta::eSuccess)
        return charta::eFailure;

    if (WriteByte(byte2) != charta::eSuccess)
        return charta::eFailure;

    if (WriteByte(byte3) != charta::eSuccess)
        return charta::eFailure;

    return charta::eSuccess;
}

EStatusCode CFFPrimitiveWriter::Write4ByteUnsigned(unsigned long inValue)
{
    uint8_t byte1 = (inValue >> 24) & 0xff;
    uint8_t byte2 = (inValue >> 16) & 0xff;
    uint8_t byte3 = (inValue >> 8) & 0xff;
    uint8_t byte4 = inValue & 0xff;

    if (WriteByte(byte1) != charta::eSuccess)
        return charta::eFailure;

    if (WriteByte(byte2) != charta::eSuccess)
        return charta::eFailure;

    if (WriteByte(byte3) != charta::eSuccess)
        return charta::eFailure;

    if (WriteByte(byte4) != charta::eSuccess)
        return charta::eFailure;

    return charta::eSuccess;
}

EStatusCode CFFPrimitiveWriter::WriteOffSize(uint8_t inValue)
{
    return WriteCard8(inValue);
}

EStatusCode CFFPrimitiveWriter::WriteSID(uint16_t inValue)
{
    return WriteCard16(inValue);
}

EStatusCode CFFPrimitiveWriter::WriteDictOperator(uint16_t inOperator)
{
    if (((inOperator >> 8) & 0xff) == 12)
        return WriteCard16(inOperator);
    return WriteCard8((uint8_t)(inOperator & 0xff));
}

EStatusCode CFFPrimitiveWriter::WriteDictOperand(const DictOperand &inOperand)
{
    if (inOperand.IsInteger)
        return WriteIntegerOperand(inOperand.IntegerValue);
    return WriteRealOperand(inOperand.RealValue, inOperand.RealValueFractalEnd);
}

EStatusCode CFFPrimitiveWriter::WriteDictItems(uint16_t inOperator, const DictOperandList &inOperands)
{
    EStatusCode status = charta::eSuccess;
    auto it = inOperands.begin();

    for (; it != inOperands.end() && charta::eSuccess == status; ++it)
        status = WriteDictOperand(*it);
    if (charta::eSuccess == status)
        status = WriteDictOperator(inOperator);

    return status;
}

EStatusCode CFFPrimitiveWriter::WriteIntegerOperand(long inValue)
{
    if (-107 <= inValue && inValue <= 107)
    {
        return WriteByte((uint8_t)(inValue + 139));
    }
    if (108 <= inValue && inValue <= 1131)
    {
        uint8_t byte0, byte1;

        inValue -= 108;
        byte0 = ((inValue >> 8) & 0xff) + 247;
        byte1 = inValue & 0xff;

        if (WriteByte(byte0) != charta::eSuccess)
            return charta::eFailure;

        if (WriteByte(byte1) != charta::eSuccess)
            return charta::eFailure;
    }
    else if (-1131 <= inValue && inValue <= -108)
    {
        uint8_t byte0, byte1;

        inValue = -(inValue + 108);

        byte0 = ((inValue >> 8) & 0xff) + 251;
        byte1 = inValue & 0xff;

        if (WriteByte(byte0) != charta::eSuccess)
            return charta::eFailure;

        if (WriteByte(byte1) != charta::eSuccess)
            return charta::eFailure;
    }
    else if (-32768 <= inValue && inValue <= 32767)
    {
        uint8_t byte1, byte2;

        byte1 = (inValue >> 8) & 0xff;
        byte2 = inValue & 0xff;

        if (WriteByte(28) != charta::eSuccess)
            return charta::eFailure;

        if (WriteByte(byte1) != charta::eSuccess)
            return charta::eFailure;

        if (WriteByte(byte2) != charta::eSuccess)
            return charta::eFailure;
    }
    else //  -2^31 <= inValue <= 2^31 - 1
    {
        return Write5ByteDictInteger(inValue);
    }
    return charta::eSuccess;
}

EStatusCode CFFPrimitiveWriter::Write5ByteDictInteger(long inValue)
{
    uint8_t byte1, byte2, byte3, byte4;

    byte1 = (inValue >> 24) & 0xff;
    byte2 = (inValue >> 16) & 0xff;
    byte3 = (inValue >> 8) & 0xff;
    byte4 = inValue & 0xff;

    if (WriteByte(29) != charta::eSuccess)
        return charta::eFailure;

    if (WriteByte(byte1) != charta::eSuccess)
        return charta::eFailure;

    if (WriteByte(byte2) != charta::eSuccess)
        return charta::eFailure;

    if (WriteByte(byte3) != charta::eSuccess)
        return charta::eFailure;

    if (WriteByte(byte4) != charta::eSuccess)
        return charta::eFailure;

    return charta::eSuccess;
}

EStatusCode CFFPrimitiveWriter::WriteRealOperand(double inValue, long inFractalLength)
{
    // first, calculate the proper formatting

    bool minusSign = inValue < 0;
    bool minusExponent = false;
    bool plusExponent = false;
    uint16_t exponentSize = 0;

    if (minusSign)
        inValue = -inValue;

    double integerValue = floor(inValue);
    double fractalValue = inValue - integerValue;

    if (0 == fractalValue)
    {
        if (long(integerValue) % 1000 == 0 && integerValue >= 1000) // bother only if larger than 1000
        {
            plusExponent = true;
            while (long(integerValue) % 10 == 0)
            {
                ++exponentSize;
                integerValue = integerValue / 10;
            }
        }
    }
    else if (0 == integerValue)
    {
        if (fractalValue <= 0.001) // bother only if < 0.001
        {
            minusExponent = true;
            while (fractalValue < 0.1)
            {
                ++exponentSize;
                fractalValue = fractalValue * 10;
            }
        }
    }

    // now let's get to work
    if (WriteByte(30) != charta::eSuccess)
        return charta::eFailure;

    // first, take care of minus sign
    uint8_t buffer = minusSign ? 0xe0 : 0;
    bool usedFirst = minusSign;

    // Integer part
    if (integerValue != 0)
    {
        if (WriteIntegerOfReal(integerValue, buffer, usedFirst) != charta::eSuccess)
            return charta::eFailure;
    }
    else
    {
        if (SetOrWriteNibble(0, buffer, usedFirst) != charta::eSuccess)
            return charta::eFailure;
    }

    // Fractal part (if there was an integer or not)
    if (fractalValue != 0 && inFractalLength > 0)
    {
        if (SetOrWriteNibble(0xa, buffer, usedFirst) != charta::eSuccess)
            return charta::eFailure;

        while (fractalValue != 0 && inFractalLength > 0)
        {
            if (SetOrWriteNibble((uint8_t)floor(fractalValue * 10), buffer, usedFirst) != charta::eSuccess)
                return charta::eFailure;
            fractalValue = fractalValue * 10 - floor(fractalValue * 10);
            --inFractalLength;
        }
    }

    // now, if there's any exponent, write it
    if (minusExponent)
    {
        if (SetOrWriteNibble(0xc, buffer, usedFirst) != charta::eSuccess)
            return charta::eFailure;
        if (WriteIntegerOfReal(exponentSize, buffer, usedFirst) != charta::eSuccess)
            return charta::eFailure;
    }
    if (plusExponent)
    {
        if (SetOrWriteNibble(0xb, buffer, usedFirst) != charta::eSuccess)
            return charta::eFailure;
        if (WriteIntegerOfReal(exponentSize, buffer, usedFirst) != charta::eSuccess)
            return charta::eFailure;
    }

    // final f or ff
    if (usedFirst)
        return SetOrWriteNibble(0xf, buffer, usedFirst);
    return WriteByte(0xff);
}

EStatusCode CFFPrimitiveWriter::SetOrWriteNibble(uint8_t inValue, uint8_t &ioBuffer, bool &ioUsedFirst)
{
    EStatusCode status = charta::eSuccess;
    if (ioUsedFirst)
    {
        ioBuffer |= inValue;
        status = WriteByte(ioBuffer);
        ioBuffer = 0;
        ioUsedFirst = false;
    }
    else
    {
        ioBuffer = (inValue << 4) & 0xf0;
        ioUsedFirst = true;
    }
    return status;
}

EStatusCode CFFPrimitiveWriter::WriteIntegerOfReal(double inIntegerValue, uint8_t &ioBuffer, bool &ioUsedFirst)
{
    if (0 == inIntegerValue)
        return charta::eSuccess;

    EStatusCode status = WriteIntegerOfReal(floor(inIntegerValue / 10), ioBuffer, ioUsedFirst);
    if (status != charta::eSuccess)
        return charta::eFailure;

    return SetOrWriteNibble((uint8_t)(long(inIntegerValue) % 10), ioBuffer, ioUsedFirst);
}

uint8_t BytesPad5[5] = {'0', '0', '0', '0', '0'};
EStatusCode CFFPrimitiveWriter::Pad5Bytes()
{
    return Write(BytesPad5, 5);
}

uint8_t BytePad[1] = {'0'};
EStatusCode CFFPrimitiveWriter::PadNBytes(uint16_t inBytesToPad)
{
    EStatusCode status = charta::eSuccess;

    for (uint16_t i = 0; i < inBytesToPad && charta::eSuccess == status; ++i)
        Write(BytePad, 1);
    return status;
}
