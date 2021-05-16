/*
   Source File : CharStringType2Interpreter.h


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

#include "CharStringDefinitions.h"
#include "IType2InterpreterImplementation.h"

#include <vector>

typedef std::vector<CharStringOperand> CharStringOperandVector;

class CharStringType2Interpreter
{
  public:
    CharStringType2Interpreter(void);
    ~CharStringType2Interpreter(void);

    PDFHummus::EStatusCode Intepret(const CharString &inCharStringToIntepret,
                                    IType2InterpreterImplementation *inImplementationHelper);

  private:
    CharStringOperandList mOperandStack;
    unsigned short mStemsCount;
    IType2InterpreterImplementation *mImplementationHelper;
    bool mGotEndChar;
    CharStringOperandVector mStorage;
    bool mCheckedWidth;

    PDFHummus::EStatusCode ProcessCharString(uint8_t *inCharString, long long inCharStringLength);
    bool IsOperator(uint8_t *inProgramCounter);
    uint8_t *InterpretNumber(uint8_t *inProgramCounter);
    uint8_t *InterpretOperator(uint8_t *inProgramCounter, bool &outGotEndExecutionCommand);

    PDFHummus::EStatusCode ClearNFromStack(unsigned short inCount);
    void ClearStack();
    void CheckWidth();

    uint8_t *InterpretHStem(uint8_t *inProgramCounter);
    uint8_t *InterpretVStem(uint8_t *inProgramCounter);
    uint8_t *InterpretVMoveto(uint8_t *inProgramCounter);
    uint8_t *InterpretRLineto(uint8_t *inProgramCounter);
    uint8_t *InterpretHLineto(uint8_t *inProgramCounter);
    uint8_t *InterpretVLineto(uint8_t *inProgramCounter);
    uint8_t *InterpretRRCurveto(uint8_t *inProgramCounter);
    uint8_t *InterpretCallSubr(uint8_t *inProgramCounter);
    uint8_t *InterpretReturn(uint8_t *inProgramCounter);
    uint8_t *InterpretEndChar(uint8_t *inProgramCounter);
    uint8_t *InterpretHStemHM(uint8_t *inProgramCounter);
    uint8_t *InterpretHintMask(uint8_t *inProgramCounter);
    uint8_t *InterpretCntrMask(uint8_t *inProgramCounter);
    uint8_t *InterpretRMoveto(uint8_t *inProgramCounter);
    uint8_t *InterpretHMoveto(uint8_t *inProgramCounter);
    uint8_t *InterpretVStemHM(uint8_t *inProgramCounter);
    uint8_t *InterpretRCurveLine(uint8_t *inProgramCounter);
    uint8_t *InterpretRLineCurve(uint8_t *inProgramCounter);
    uint8_t *InterpretVVCurveto(uint8_t *inProgramCounter);
    uint8_t *InterpretHHCurveto(uint8_t *inProgramCounter);
    uint8_t *InterpretCallGSubr(uint8_t *inProgramCounter);
    uint8_t *InterpretVHCurveto(uint8_t *inProgramCounter);
    uint8_t *InterpretHVCurveto(uint8_t *inProgramCounter);
    uint8_t *InterpretAnd(uint8_t *inProgramCounter);
    uint8_t *InterpretOr(uint8_t *inProgramCounter);
    uint8_t *InterpretNot(uint8_t *inProgramCounter);
    uint8_t *InterpretAbs(uint8_t *inProgramCounter);
    uint8_t *InterpretAdd(uint8_t *inProgramCounter);
    uint8_t *InterpretSub(uint8_t *inProgramCounter);
    uint8_t *InterpretDiv(uint8_t *inProgramCounter);
    uint8_t *InterpretNeg(uint8_t *inProgramCounter);
    uint8_t *InterpretEq(uint8_t *inProgramCounter);
    uint8_t *InterpretDrop(uint8_t *inProgramCounter);
    uint8_t *InterpretPut(uint8_t *inProgramCounter);
    uint8_t *InterpretGet(uint8_t *inProgramCounter);
    uint8_t *InterpretIfelse(uint8_t *inProgramCounter);
    uint8_t *InterpretRandom(uint8_t *inProgramCounter);
    uint8_t *InterpretMul(uint8_t *inProgramCounter);
    uint8_t *InterpretSqrt(uint8_t *inProgramCounter);
    uint8_t *InterpretDup(uint8_t *inProgramCounter);
    uint8_t *InterpretExch(uint8_t *inProgramCounter);
    uint8_t *InterpretIndex(uint8_t *inProgramCounter);
    uint8_t *InterpretRoll(uint8_t *inProgramCounter);
    uint8_t *InterpretHFlex(uint8_t *inProgramCounter);
    uint8_t *InterpretFlex(uint8_t *inProgramCounter);
    uint8_t *InterpretHFlex1(uint8_t *inProgramCounter);
    uint8_t *InterpretFlex1(uint8_t *inProgramCounter);
};
