/*
   Source File : CharStringType1Interpreter.h


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
#include "io/InputCharStringDecodeStream.h"

class CharStringType1Interpreter
{
  public:
    CharStringType1Interpreter(void);
    ~CharStringType1Interpreter(void);

    charta::EStatusCode Intepret(const Type1CharString &inCharStringToIntepret,
                                 IType1InterpreterImplementation *inImplementationHelper);

  private:
    LongList mOperandStack;
    IType1InterpreterImplementation *mImplementationHelper;
    bool mGotEndChar;
    LongList mPostScriptOperandStack;

    charta::EStatusCode ProcessCharString(InputCharStringDecodeStream *inCharStringToIntepret);
    bool IsOperator(uint8_t inBuffer);
    charta::EStatusCode InterpretOperator(uint8_t inBuffer, InputCharStringDecodeStream *inCharStringToIntepret,
                                          bool &outGotEndExecutionCommand);
    charta::EStatusCode InterpretNumber(uint8_t inBuffer, InputCharStringDecodeStream *inCharStringToIntepret);
    void ClearStack();
    charta::EStatusCode DefaultCallOtherSubr();

    charta::EStatusCode InterpretHStem();
    charta::EStatusCode InterpretVStem();
    charta::EStatusCode InterpretVMoveto();
    charta::EStatusCode InterpretRLineto();
    charta::EStatusCode InterpretHLineto();
    charta::EStatusCode InterpretVLineto();
    charta::EStatusCode InterpretRRCurveto();
    charta::EStatusCode InterpretClosePath();
    charta::EStatusCode InterpretCallSubr();
    charta::EStatusCode InterpretReturn();
    charta::EStatusCode InterpretHsbw();
    charta::EStatusCode InterpretEndChar();
    charta::EStatusCode InterpretRMoveto();
    charta::EStatusCode InterpretHMoveto();
    charta::EStatusCode InterpretVHCurveto();
    charta::EStatusCode InterpretHVCurveto();
    charta::EStatusCode InterpretDotSection();
    charta::EStatusCode InterpretVStem3();
    charta::EStatusCode InterpretHStem3();
    charta::EStatusCode InterpretSeac();
    charta::EStatusCode InterpretSbw();
    charta::EStatusCode InterpretDiv();
    charta::EStatusCode InterpretCallOtherSubr();
    charta::EStatusCode InterpretPop();
    charta::EStatusCode InterpretSetCurrentPoint();
};
