/*
   Source File : CFFPrimitiveWriter.h


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
#include "DictOperand.h"
#include "EStatusCode.h"
#include "io/IByteWriter.h"

class CFFPrimitiveWriter
{
  public:
    CFFPrimitiveWriter(IByteWriter *inCFFOutput = NULL);
    ~CFFPrimitiveWriter(void) = default;

    void SetStream(IByteWriter *inCFFOutput);

    charta::EStatusCode GetInternalState();

    charta::EStatusCode WriteByte(uint8_t inValue);
    charta::EStatusCode Write(const uint8_t *inBuffer, size_t inBufferSize);

    // basic CFF values
    charta::EStatusCode WriteCard8(uint8_t inValue);
    charta::EStatusCode WriteCard16(uint16_t inValue);
    // set offsize to write offsets
    void SetOffSize(uint8_t inOffSize);
    charta::EStatusCode WriteOffset(unsigned long inValue);
    charta::EStatusCode WriteOffSize(uint8_t inValue);
    charta::EStatusCode WriteSID(uint16_t inValue);

    // dict data
    charta::EStatusCode WriteDictOperator(uint16_t inOperator);
    charta::EStatusCode WriteDictOperand(const DictOperand &inOperand);
    charta::EStatusCode WriteDictItems(uint16_t inOperator, const DictOperandList &inOperands);
    charta::EStatusCode WriteIntegerOperand(long inValue);
    charta::EStatusCode Write5ByteDictInteger(long inValue);
    charta::EStatusCode WriteRealOperand(double inValue, long inFractalLength = 10);

    charta::EStatusCode Pad5Bytes();
    charta::EStatusCode PadNBytes(uint16_t inBytesToPad);

  private:
    IByteWriter *mCFFOutput;
    charta::EStatusCode mInternalState;
    uint8_t mCurrentOffsize;

    charta::EStatusCode Write3ByteUnsigned(unsigned long inValue);
    charta::EStatusCode Write4ByteUnsigned(unsigned long inValue);
    charta::EStatusCode WriteIntegerOfReal(double inIntegerValue, uint8_t &ioBuffer, bool &ioUsedFirst);
    charta::EStatusCode SetOrWriteNibble(uint8_t inValue, uint8_t &ioBuffer, bool &ioUsedFirst);
};
