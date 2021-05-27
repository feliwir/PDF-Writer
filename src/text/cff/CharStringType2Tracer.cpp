/*
   Source File : CharStringType2Tracer.cpp


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
#include "text/cff/CharStringType2Tracer.h"
#include "BoxingBase.h"
#include "SafeBufferMacrosDefs.h"
#include "Trace.h"
#include "io/IByteWriter.h"
#include "text/cff/CFFFileInput.h"
#include "text/cff/CharStringType2Interpreter.h"

using namespace charta;

CharStringType2Tracer::CharStringType2Tracer() = default;

EStatusCode CharStringType2Tracer::TraceGlyphProgram(uint16_t inFontIndex, uint16_t inGlyphIndex,
                                                     CFFFileInput *inCFFFileInput, IByteWriter *inWriter)
{
    CharStringType2Interpreter interpreter;
    EStatusCode status = inCFFFileInput->PrepareForGlyphIntepretation(inFontIndex, inGlyphIndex);

    mWriter = inWriter;
    mHelper = inCFFFileInput;
    mPrimitiveWriter.SetStreamForWriting(inWriter);
    mStemsCount = 0;

    do
    {
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CharStringType2Tracer::Trace, Exception, cannot prepare for glyph interpretation");
            break;
        }

        CharString *charString = inCFFFileInput->GetGlyphCharString(inFontIndex, inGlyphIndex);
        if (charString == nullptr)
        {
            TRACE_LOG("CharStringType2Tracer::Trace, Exception, cannot find glyph index");
            break;
        }

        status = interpreter.Intepret(*charString, this);

    } while (false);
    return status;
}

EStatusCode CharStringType2Tracer::ReadCharString(long long inCharStringStart, long long inCharStringEnd,
                                                  uint8_t **outCharString)
{
    return mHelper->ReadCharString(inCharStringStart, inCharStringEnd, outCharString);
}

EStatusCode CharStringType2Tracer::Type2InterpretNumber(const CharStringOperand &inOperand)
{
    if (inOperand.IsInteger)
        mPrimitiveWriter.WriteInteger(inOperand.IntegerValue);
    else
        mPrimitiveWriter.WriteDouble(inOperand.RealValue);
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Hstem(const CharStringOperandList &inOperandList)
{
    mStemsCount += (uint16_t)(inOperandList.size() / 2);

    mPrimitiveWriter.WriteKeyword("hstem");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Vstem(const CharStringOperandList &inOperandList)
{
    mStemsCount += (uint16_t)(inOperandList.size() / 2);

    mPrimitiveWriter.WriteKeyword("vstem");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Vmoveto(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("vstem");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Rlineto(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("rlineto");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Hlineto(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("hlineto");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Vlineto(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("vlineto");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2RRCurveto(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("rrcurveto");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Return(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("return");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Endchar(const CharStringOperandList & /*inOperandList*/)
{
    // no need to call the CFFFileInput endchar here. that call is used for dependencies check alone
    // and provides for CFFFileInput own intepreter implementation.

    mPrimitiveWriter.WriteKeyword("endchar");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Hstemhm(const CharStringOperandList &inOperandList)
{
    mStemsCount += (uint16_t)(inOperandList.size() / 2);

    mPrimitiveWriter.WriteKeyword("hstemhm");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Hintmask(const CharStringOperandList &inOperandList, uint8_t *inProgramCounter)
{
    mStemsCount += (uint16_t)(inOperandList.size() / 2);

    WriteStemMask(inProgramCounter);
    mPrimitiveWriter.WriteKeyword("hintmask");
    return charta::eSuccess;
}

void CharStringType2Tracer::WriteStemMask(uint8_t *inProgramCounter)
{
    uint16_t maskSize = mStemsCount / 8 + (mStemsCount % 8 != 0 ? 1 : 0);
    char buffer[3];

    mWriter->Write((const uint8_t *)"(0x", 1);
    for (uint16_t i = 0; i < maskSize; ++i)
    {
        SAFE_SPRINTF_1(buffer, 3, "%X", inProgramCounter[i]);
        mWriter->Write((const uint8_t *)buffer, 2);
    }

    mWriter->Write((const uint8_t *)")", 1);
}

EStatusCode CharStringType2Tracer::Type2Cntrmask(const CharStringOperandList & /*inOperandList*/,
                                                 uint8_t *inProgramCounter)
{
    WriteStemMask(inProgramCounter);
    mPrimitiveWriter.WriteKeyword("cntrmask");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Rmoveto(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("rmoveto");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Hmoveto(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("hmoveto");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Vstemhm(const CharStringOperandList &inOperandList)
{
    mStemsCount += (uint16_t)(inOperandList.size() / 2);

    mPrimitiveWriter.WriteKeyword("vstemhm");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Rcurveline(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("rcurveline");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Rlinecurve(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("rlinecurve");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Vvcurveto(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("vvcurveto");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Hvcurveto(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("hvcurveto");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Hhcurveto(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("hhcurveto");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Vhcurveto(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("vhcurveto");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Hflex(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("hflex");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Hflex1(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("hflex1");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Flex(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("flex");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Flex1(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("flex1");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2And(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("and");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Or(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("or");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Not(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("not");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Abs(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("abs");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Add(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("add");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Sub(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("sub");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Div(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("div");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Neg(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("neg");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Eq(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("eq");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Drop(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("drop");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Put(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("put");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Get(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("get");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Ifelse(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("ifelse");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Random(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("random");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Mul(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("mul");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Sqrt(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("sqrt");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Dup(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("dup");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Exch(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("exch");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Index(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("index");
    return charta::eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Roll(const CharStringOperandList & /*inOperandList*/)
{
    mPrimitiveWriter.WriteKeyword("roll");
    return charta::eSuccess;
}

CharString *CharStringType2Tracer::GetLocalSubr(long inSubrIndex)
{
    mPrimitiveWriter.WriteKeyword("callsubr");

    return mHelper->GetLocalSubr(inSubrIndex);
}

CharString *CharStringType2Tracer::GetGlobalSubr(long inSubrIndex)
{
    mPrimitiveWriter.WriteKeyword("callgsubr");

    return mHelper->GetGlobalSubr(inSubrIndex);
}
