/*
   Source File : IType2InterpreterImplementation.h


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
#include "text/cff/CharStringDefinitions.h"

class IType2InterpreterImplementation
{
  public:
    virtual charta::EStatusCode ReadCharString(long long inCharStringStart, long long inCharStringEnd,
                                               uint8_t **outCharString) = 0;

    // events in the code
    virtual charta::EStatusCode Type2InterpretNumber(const CharStringOperand &inOperand) = 0;
    virtual charta::EStatusCode Type2Hstem(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Vstem(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Vmoveto(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Rlineto(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Hlineto(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Vlineto(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2RRCurveto(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Return(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Endchar(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Hstemhm(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Hintmask(const CharStringOperandList &inOperandList,
                                              uint8_t *inProgramCounter) = 0;
    virtual charta::EStatusCode Type2Cntrmask(const CharStringOperandList &inOperandList,
                                              uint8_t *inProgramCounter) = 0;
    virtual charta::EStatusCode Type2Rmoveto(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Hmoveto(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Vstemhm(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Rcurveline(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Rlinecurve(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Vvcurveto(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Hvcurveto(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Hhcurveto(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Vhcurveto(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Hflex(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Hflex1(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Flex(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Flex1(const CharStringOperandList &inOperandList) = 0;

    virtual charta::EStatusCode Type2And(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Or(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Not(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Abs(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Add(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Sub(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Div(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Neg(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Eq(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Drop(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Put(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Get(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Ifelse(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Random(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Mul(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Sqrt(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Dup(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Exch(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Index(const CharStringOperandList &inOperandList) = 0;
    virtual charta::EStatusCode Type2Roll(const CharStringOperandList &inOperandList) = 0;

    virtual CharString *GetLocalSubr(long inSubrIndex) = 0;  // you should bias the index !!
    virtual CharString *GetGlobalSubr(long inSubrIndex) = 0; // you should bias the index !!
};

class Type2InterpreterImplementationAdapter : public IType2InterpreterImplementation
{
  public:
    virtual charta::EStatusCode ReadCharString(long long inCharStringStart, long long inCharStringEnd,
                                               uint8_t **outCharString)
    {
        (void)inCharStringStart;
        (void)inCharStringEnd;
        return charta::eFailure;
    }

    virtual charta::EStatusCode Type2InterpretNumber(const CharStringOperand &inOperand)
    {
        (void)inOperand;
        return charta::eSuccess;
    };
    virtual charta::EStatusCode Type2Hstem(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Vstem(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Vmoveto(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Rlineto(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Hlineto(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Vlineto(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2RRCurveto(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Return(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Endchar(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Hstemhm(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Hintmask(const CharStringOperandList &inOperandList, uint8_t *inProgramCounter)
    {
        (void)inOperandList;
        (void)(void) inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Cntrmask(const CharStringOperandList &inOperandList, uint8_t *inProgramCounter)
    {
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Rmoveto(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Hmoveto(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Vstemhm(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Rcurveline(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Rlinecurve(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Vvcurveto(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Hvcurveto(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Hhcurveto(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Vhcurveto(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Hflex(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Hflex1(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Flex(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Flex1(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }

    virtual charta::EStatusCode Type2And(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Or(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Not(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Abs(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Add(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Sub(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Div(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Neg(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Eq(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Drop(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Put(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Get(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Ifelse(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Random(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Mul(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Sqrt(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Dup(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Exch(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Index(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }
    virtual charta::EStatusCode Type2Roll(const CharStringOperandList &inOperandList)
    {
        (void)inOperandList;
        return charta::eSuccess;
    }

    virtual CharString *GetLocalSubr(long inSubrIndex)
    {
        (void)inSubrIndex;
        return NULL;
    }
    virtual CharString *GetGlobalSubr(long inSubrIndex)
    {
        (void)inSubrIndex;
        return NULL;
    }
};
