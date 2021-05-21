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

    PDFHummus::EStatusCode GetInternalState();

    PDFHummus::EStatusCode WriteByte(uint8_t inValue);
    PDFHummus::EStatusCode Write(const uint8_t *inBuffer, size_t inBufferSize);

    // basic CFF values
    PDFHummus::EStatusCode WriteCard8(uint8_t inValue);
    PDFHummus::EStatusCode WriteCard16(uint16_t inValue);
    // set offsize to write offsets
    void SetOffSize(uint8_t inOffSize);
    PDFHummus::EStatusCode WriteOffset(unsigned long inValue);
    PDFHummus::EStatusCode WriteOffSize(uint8_t inValue);
    PDFHummus::EStatusCode WriteSID(uint16_t inValue);

    // dict data
    PDFHummus::EStatusCode WriteDictOperator(uint16_t inOperator);
    PDFHummus::EStatusCode WriteDictOperand(const DictOperand &inOperand);
    PDFHummus::EStatusCode WriteDictItems(uint16_t inOperator, const DictOperandList &inOperands);
    PDFHummus::EStatusCode WriteIntegerOperand(long inValue);
    PDFHummus::EStatusCode Write5ByteDictInteger(long inValue);
    PDFHummus::EStatusCode WriteRealOperand(double inValue, long inFractalLength = 10);

    PDFHummus::EStatusCode Pad5Bytes();
    PDFHummus::EStatusCode PadNBytes(uint16_t inBytesToPad);

  private:
    IByteWriter *mCFFOutput;
    PDFHummus::EStatusCode mInternalState;
    uint8_t mCurrentOffsize;

    PDFHummus::EStatusCode Write3ByteUnsigned(unsigned long inValue);
    PDFHummus::EStatusCode Write4ByteUnsigned(unsigned long inValue);
    PDFHummus::EStatusCode WriteIntegerOfReal(double inIntegerValue, uint8_t &ioBuffer, bool &ioUsedFirst);
    PDFHummus::EStatusCode SetOrWriteNibble(uint8_t inValue, uint8_t &ioBuffer, bool &ioUsedFirst);
};