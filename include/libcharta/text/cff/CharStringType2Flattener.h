/*
   Source File : CharStringType2Flattener.h


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
#pragma once

#include "EStatusCode.h"
#include "IType2InterpreterImplementation.h"

class CFFFileInput;
namespace charta
{
class IByteWriter;
}

class CharStringType2Flattener : public IType2InterpreterImplementation
{
  public:
    CharStringType2Flattener(void);
    ~CharStringType2Flattener(void);

    // will write a font program to another stream, flattening the references to subrs and gsubrs, so that
    // the charstring becomes independent (with possible references to other charachters through seac-like endchar)
    charta::EStatusCode WriteFlattenedGlyphProgram(uint16_t inFontIndex, uint16_t inGlyphIndex,
                                                   CFFFileInput *inCFFFileInput, charta::IByteWriter *inWriter);

    // IType2InterpreterImplementation implementation
    virtual charta::EStatusCode ReadCharString(long long inCharStringStart, long long inCharStringEnd,
                                               uint8_t **outCharString);
    virtual charta::EStatusCode Type2InterpretNumber(const CharStringOperand &inOperand);
    virtual charta::EStatusCode Type2Hstem(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Vstem(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Vmoveto(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Rlineto(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Hlineto(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Vlineto(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2RRCurveto(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Return(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Endchar(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Hstemhm(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Hintmask(const CharStringOperandList &inOperandList, uint8_t *inProgramCounter);
    virtual charta::EStatusCode Type2Cntrmask(const CharStringOperandList &inOperandList, uint8_t *inProgramCounter);
    virtual charta::EStatusCode Type2Rmoveto(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Hmoveto(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Vstemhm(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Rcurveline(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Rlinecurve(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Vvcurveto(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Hvcurveto(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Hhcurveto(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Vhcurveto(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Hflex(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Hflex1(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Flex(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Flex1(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2And(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Or(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Not(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Abs(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Add(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Sub(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Div(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Neg(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Eq(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Drop(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Put(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Get(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Ifelse(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Random(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Mul(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Sqrt(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Dup(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Exch(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Index(const CharStringOperandList &inOperandList);
    virtual charta::EStatusCode Type2Roll(const CharStringOperandList &inOperandList);
    virtual CharString *GetLocalSubr(long inSubrIndex);
    virtual CharString *GetGlobalSubr(long inSubrIndex);

  private:
    charta::IByteWriter *mWriter;
    CFFFileInput *mHelper;
    uint16_t mStemsCount;
    CharStringOperandList mOperandsToWrite;

    charta::EStatusCode WriteRegularOperator(uint16_t inOperatorCode);
    charta::EStatusCode WriteStemMask(uint8_t *inProgramCounter);
    charta::EStatusCode WriteCharStringOperand(const CharStringOperand &inOperand);
    charta::EStatusCode WriteCharStringOperator(uint16_t inOperatorCode);
    charta::EStatusCode WriteByte(uint8_t inValue);
    charta::EStatusCode WriteSubrOperator(uint16_t inOperatorCode);
};
