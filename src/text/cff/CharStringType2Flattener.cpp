/*
   Source File : CharStringType2Flattener.cpp


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
#include "text/cff/CharStringType2Flattener.h"
#include "Trace.h"
#include "io/IByteWriter.h"
#include "text/cff/CFFFileInput.h"
#include "text/cff/CharStringType2Interpreter.h"

#include <math.h>

using namespace charta;

CharStringType2Flattener::CharStringType2Flattener() = default;

CharStringType2Flattener::~CharStringType2Flattener() = default;

EStatusCode CharStringType2Flattener::WriteFlattenedGlyphProgram(uint16_t inFontIndex, uint16_t inGlyphIndex,
                                                                 CFFFileInput *inCFFFileInput,
                                                                 charta::IByteWriter *inWriter)
{
    CharStringType2Interpreter interpreter;
    EStatusCode status = inCFFFileInput->PrepareForGlyphIntepretation(inFontIndex, inGlyphIndex);

    mWriter = inWriter;
    mHelper = inCFFFileInput;
    mOperandsToWrite.clear();
    mStemsCount = 0;

    do
    {
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CharStringType2Flattener::Trace, Exception, cannot prepare for glyph interpretation");
            break;
        }

        CharString *charString = inCFFFileInput->GetGlyphCharString(inFontIndex, inGlyphIndex);
        if (charString == nullptr)
        {
            TRACE_LOG("CharStringType2Flattener::Trace, Exception, cannot find glyph index");
            break;
        }

        status = interpreter.Intepret(*charString, this);

        /*
            The alrogithm for writing a flattened charstring is as follows:
            1. enumerator, through interpretation, the charstring
            2. hit an operand? accumulate.
            3. hit an operator? if it's not callgsubr or callsubr just write the operand stack, and continue.
                                if it is callgsubr/callsubr pop the last element on the operand stack and write it, then
           continue.
            4. an exception would be when callgsubr/callsubr follow an operator, in which case their index operand is
           already written. just call drop.

        */

    } while (false);
    return status;
}

EStatusCode CharStringType2Flattener::ReadCharString(long long inCharStringStart, long long inCharStringEnd,
                                                     uint8_t **outCharString)
{
    return mHelper->ReadCharString(inCharStringStart, inCharStringEnd, outCharString);
}

EStatusCode CharStringType2Flattener::Type2InterpretNumber(const CharStringOperand &inOperand)
{
    mOperandsToWrite.push_back(inOperand);
    return charta::eSuccess;
}

EStatusCode CharStringType2Flattener::Type2Hstem(const CharStringOperandList &inOperandList)
{
    mStemsCount += (uint16_t)(inOperandList.size() / 2);

    return WriteRegularOperator(1);
}

EStatusCode CharStringType2Flattener::WriteRegularOperator(uint16_t inOperatorCode)
{
    auto it = mOperandsToWrite.begin();
    EStatusCode status = charta::eSuccess;

    for (; it != mOperandsToWrite.end() && charta::eSuccess == status; ++it)
        status = WriteCharStringOperand(*it);
    if (status != charta::eFailure)
        status = WriteCharStringOperator(inOperatorCode);

    mOperandsToWrite.clear();

    return status;
}

EStatusCode CharStringType2Flattener::WriteCharStringOperand(const CharStringOperand &inOperand)
{
    if (inOperand.IsInteger)
    {
        long value = inOperand.IntegerValue;

        if (-107 <= value && value <= 107)
        {
            return WriteByte((uint8_t)(value + 139));
        }
        if (108 <= value && value <= 1131)
        {
            uint8_t byte0, byte1;

            value -= 108;
            byte0 = ((value >> 8) & 0xff) + 247;
            byte1 = value & 0xff;

            if (WriteByte(byte0) != charta::eSuccess)
                return charta::eFailure;

            if (WriteByte(byte1) != charta::eSuccess)
                return charta::eFailure;
        }
        else if (-1131 <= value && value <= -108)
        {
            uint8_t byte0, byte1;

            value = -(value + 108);

            byte0 = ((value >> 8) & 0xff) + 251;
            byte1 = value & 0xff;

            if (WriteByte(byte0) != charta::eSuccess)
                return charta::eFailure;

            if (WriteByte(byte1) != charta::eSuccess)
                return charta::eFailure;
        }
        else if (-32768 <= value && value <= 32767)
        {
            uint8_t byte1, byte2;

            byte1 = (value >> 8) & 0xff;
            byte2 = value & 0xff;

            if (WriteByte(28) != charta::eSuccess)
                return charta::eFailure;

            if (WriteByte(byte1) != charta::eSuccess)
                return charta::eFailure;

            if (WriteByte(byte2) != charta::eSuccess)
                return charta::eFailure;
        }
        else
            return charta::eFailure;
    }
    else
    {
        double value = inOperand.RealValue;
        bool sign = inOperand.RealValue < 0;
        if (sign)
            value = -value;
        long integerPart = (long)floor(value);
        long realPart = (long)((value - floor(value)) * 65536);
        if (sign)
            integerPart = -integerPart;

        if (WriteByte(uint8_t(0xff)) != charta::eSuccess)
            return charta::eFailure;
        if (WriteByte(uint8_t((integerPart >> 8) & 0xff)) != charta::eSuccess)
            return charta::eFailure;
        if (WriteByte(uint8_t(integerPart & 0xff)) != charta::eSuccess)
            return charta::eFailure;

        if (WriteByte(uint8_t((realPart >> 8) & 0xff)) != charta::eSuccess)
            return charta::eFailure;
        if (WriteByte(uint8_t(realPart & 0xff)) != charta::eSuccess)
            return charta::eFailure;
    }
    return charta::eSuccess;
}

EStatusCode CharStringType2Flattener::WriteCharStringOperator(uint16_t inOperatorCode)
{
    if ((inOperatorCode & 0xff00) == 0x0c00)
    {
        if (WriteByte(0x0c) != charta::eSuccess)
            return charta::eFailure;
    }
    return WriteByte(uint8_t(inOperatorCode & 0xff));
}

EStatusCode CharStringType2Flattener::WriteByte(uint8_t inValue)
{
    return (mWriter->Write(&inValue, 1) == 1 ? charta::eSuccess : charta::eFailure);
}

EStatusCode CharStringType2Flattener::Type2Vstem(const CharStringOperandList &inOperandList)
{
    mStemsCount += (uint16_t)(inOperandList.size() / 2);

    return WriteRegularOperator(3);
}

EStatusCode CharStringType2Flattener::Type2Vmoveto(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(4);
}

EStatusCode CharStringType2Flattener::Type2Rlineto(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(5);
}

EStatusCode CharStringType2Flattener::Type2Hlineto(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(6);
}

EStatusCode CharStringType2Flattener::Type2Vlineto(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(7);
}

EStatusCode CharStringType2Flattener::Type2RRCurveto(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(8);
}

EStatusCode CharStringType2Flattener::Type2Return(const CharStringOperandList & /*inOperandList*/)
{
    // ignore returns
    return charta::eSuccess;
}

EStatusCode CharStringType2Flattener::Type2Endchar(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(14);
}

EStatusCode CharStringType2Flattener::Type2Hstemhm(const CharStringOperandList &inOperandList)
{
    mStemsCount += (uint16_t)(inOperandList.size() / 2);

    return WriteRegularOperator(18);
}

EStatusCode CharStringType2Flattener::Type2Hintmask(const CharStringOperandList &inOperandList,
                                                    uint8_t *inProgramCounter)
{
    mStemsCount += (uint16_t)(inOperandList.size() / 2);

    if (WriteRegularOperator(19) != charta::eSuccess)
        return charta::eFailure;

    return WriteStemMask(inProgramCounter);
}

EStatusCode CharStringType2Flattener::WriteStemMask(uint8_t *inProgramCounter)
{
    uint16_t maskSize = mStemsCount / 8 + (mStemsCount % 8 != 0 ? 1 : 0);

    return mWriter->Write(inProgramCounter, maskSize) != maskSize ? charta::eFailure : charta::eSuccess;
}

EStatusCode CharStringType2Flattener::Type2Cntrmask(const CharStringOperandList & /*inOperandList*/,
                                                    uint8_t *inProgramCounter)
{
    if (WriteRegularOperator(20) != charta::eSuccess)
        return charta::eFailure;

    return WriteStemMask(inProgramCounter);
}

EStatusCode CharStringType2Flattener::Type2Rmoveto(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(21);
}

EStatusCode CharStringType2Flattener::Type2Hmoveto(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(22);
}

EStatusCode CharStringType2Flattener::Type2Vstemhm(const CharStringOperandList &inOperandList)
{
    mStemsCount += (uint16_t)(inOperandList.size() / 2);

    return WriteRegularOperator(23);
}

EStatusCode CharStringType2Flattener::Type2Rcurveline(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(24);
}

EStatusCode CharStringType2Flattener::Type2Rlinecurve(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(25);
}

EStatusCode CharStringType2Flattener::Type2Vvcurveto(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(26);
}

EStatusCode CharStringType2Flattener::Type2Hvcurveto(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(31);
}

EStatusCode CharStringType2Flattener::Type2Hhcurveto(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(27);
}

EStatusCode CharStringType2Flattener::Type2Vhcurveto(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(30);
}

EStatusCode CharStringType2Flattener::Type2Hflex(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c22);
}

EStatusCode CharStringType2Flattener::Type2Hflex1(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c24);
}

EStatusCode CharStringType2Flattener::Type2Flex(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c23);
}

EStatusCode CharStringType2Flattener::Type2Flex1(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c25);
}

EStatusCode CharStringType2Flattener::Type2And(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c03);
}

EStatusCode CharStringType2Flattener::Type2Or(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c04);
}

EStatusCode CharStringType2Flattener::Type2Not(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c05);
}

EStatusCode CharStringType2Flattener::Type2Abs(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c09);
}

EStatusCode CharStringType2Flattener::Type2Add(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c0a);
}

EStatusCode CharStringType2Flattener::Type2Sub(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c0b);
}

EStatusCode CharStringType2Flattener::Type2Div(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c0c);
}

EStatusCode CharStringType2Flattener::Type2Neg(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c0e);
}

EStatusCode CharStringType2Flattener::Type2Eq(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c0f);
}

EStatusCode CharStringType2Flattener::Type2Drop(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c12);
}

EStatusCode CharStringType2Flattener::Type2Put(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c14);
}

EStatusCode CharStringType2Flattener::Type2Get(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c15);
}

EStatusCode CharStringType2Flattener::Type2Ifelse(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c16);
}

EStatusCode CharStringType2Flattener::Type2Random(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c17);
}

EStatusCode CharStringType2Flattener::Type2Mul(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c18);
}

EStatusCode CharStringType2Flattener::Type2Sqrt(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c1a);
}

EStatusCode CharStringType2Flattener::Type2Dup(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c1b);
}

EStatusCode CharStringType2Flattener::Type2Exch(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c1c);
}

EStatusCode CharStringType2Flattener::Type2Index(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c1d);
}

EStatusCode CharStringType2Flattener::Type2Roll(const CharStringOperandList & /*inOperandList*/)
{
    return WriteRegularOperator(0x0c1e);
}

CharString *CharStringType2Flattener::GetLocalSubr(long inSubrIndex)
{
    if (WriteSubrOperator(10) != charta::eSuccess)
        return nullptr;

    return mHelper->GetLocalSubr(inSubrIndex);
}

EStatusCode CharStringType2Flattener::WriteSubrOperator(uint16_t /*inOperatorCode*/)
{
    if (!mOperandsToWrite.empty())
    {
        EStatusCode status = charta::eSuccess;
        mOperandsToWrite.pop_back(); // pop back parameter, which is the subr index

        // now continue writing all operands
        auto it = mOperandsToWrite.begin();

        for (; it != mOperandsToWrite.end() && charta::eSuccess == status; ++it)
            status = WriteCharStringOperand(*it);

        mOperandsToWrite.clear();
        return status;
    }
    // no current operands. either result of calculation or just multiple operators one of the other
    return WriteCharStringOperator(0x0c12); // write a "drop" command for the subr index
}

CharString *CharStringType2Flattener::GetGlobalSubr(long inSubrIndex)
{
    if (WriteSubrOperator(29) != charta::eSuccess)
        return nullptr;

    return mHelper->GetGlobalSubr(inSubrIndex);
}
