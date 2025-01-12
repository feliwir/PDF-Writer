/*
   Source File : CharStringType1Tracer.h


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
#include "IType1InterpreterImplementation.h"
#include "PrimitiveObjectsWriter.h"
#include <string>

class Type1Input;
namespace charta
{
class IByteWriter;
}

class CharStringType1Tracer : public IType1InterpreterImplementation
{
  public:
    CharStringType1Tracer(void);
    ~CharStringType1Tracer(void) = default;

    charta::EStatusCode TraceGlyphProgram(uint8_t inGlyphIndex, Type1Input *inType1Input,
                                          charta::IByteWriter *inWriter);
    charta::EStatusCode TraceGlyphProgram(const std::string &inGlyphName, Type1Input *inType1Input,
                                          charta::IByteWriter *inWriter);

    // IType1InterpreterImplementation
    virtual charta::EStatusCode Type1Hstem(const LongList &inOperandList);
    virtual charta::EStatusCode Type1Vstem(const LongList &inOperandList);
    virtual charta::EStatusCode Type1VMoveto(const LongList &inOperandList);
    virtual charta::EStatusCode Type1RLineto(const LongList &inOperandList);
    virtual charta::EStatusCode Type1HLineto(const LongList &inOperandList);
    virtual charta::EStatusCode Type1VLineto(const LongList &inOperandList);
    virtual charta::EStatusCode Type1RRCurveto(const LongList &inOperandList);
    virtual charta::EStatusCode Type1ClosePath(const LongList &inOperandList);
    virtual Type1CharString *GetSubr(long inSubrIndex);
    virtual charta::EStatusCode Type1Return(const LongList &inOperandList);
    virtual charta::EStatusCode Type1Hsbw(const LongList &inOperandList);
    virtual charta::EStatusCode Type1Endchar(const LongList &inOperandList);
    virtual charta::EStatusCode Type1RMoveto(const LongList &inOperandList);
    virtual charta::EStatusCode Type1HMoveto(const LongList &inOperandList);
    virtual charta::EStatusCode Type1VHCurveto(const LongList &inOperandList);
    virtual charta::EStatusCode Type1HVCurveto(const LongList &inOperandList);
    virtual charta::EStatusCode Type1DotSection(const LongList &inOperandList);
    virtual charta::EStatusCode Type1VStem3(const LongList &inOperandList);
    virtual charta::EStatusCode Type1HStem3(const LongList &inOperandList);
    virtual charta::EStatusCode Type1Seac(const LongList &inOperandList);
    virtual charta::EStatusCode Type1Sbw(const LongList &inOperandList);
    virtual charta::EStatusCode Type1Div(const LongList &inOperandList);
    virtual bool IsOtherSubrSupported(long inOtherSubrsIndex);
    virtual charta::EStatusCode CallOtherSubr(const LongList &inOperandList, LongList &outPostScriptOperandStack);
    virtual charta::EStatusCode Type1Pop(const LongList &inOperandList, const LongList &inPostScriptOperandStack);
    virtual charta::EStatusCode Type1SetCurrentPoint(const LongList &inOperandList);
    virtual charta::EStatusCode Type1InterpretNumber(long inOperand);
    virtual unsigned long GetLenIV();

  private:
    charta::IByteWriter *mWriter;
    Type1Input *mHelper;
    PrimitiveObjectsWriter mPrimitiveWriter;
};
