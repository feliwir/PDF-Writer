/*
   Source File : CFFPrimitiveReader.h


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
#include "io/IByteReaderWithPosition.h"

class CFFPrimitiveReader
{
  public:
    CFFPrimitiveReader(IByteReaderWithPosition *inCFFFile = NULL);
    ~CFFPrimitiveReader(void) = default;

    void SetStream(IByteReaderWithPosition *inCFFFile);
    void SetOffset(long long inNewOffset);
    void Skip(size_t inToSkip);
    long long GetCurrentPosition();
    PDFHummus::EStatusCode GetInternalState();

    PDFHummus::EStatusCode ReadByte(uint8_t &outValue);
    PDFHummus::EStatusCode Read(uint8_t *ioBuffer, size_t inBufferSize);

    // basic CFF values
    PDFHummus::EStatusCode ReadCard8(uint8_t &outValue);
    PDFHummus::EStatusCode ReadCard16(uint16_t &outValue);
    // set offsize to read offsets
    void SetOffSize(uint8_t inOffSize);
    PDFHummus::EStatusCode ReadOffset(unsigned long &outValue);
    PDFHummus::EStatusCode ReadOffSize(uint8_t &outValue);
    PDFHummus::EStatusCode ReadSID(uint16_t &outValue);

    // dict data
    bool IsDictOperator(uint8_t inCandidate);
    PDFHummus::EStatusCode ReadDictOperator(uint8_t inFirstByte, uint16_t &outOperator);
    PDFHummus::EStatusCode ReadDictOperand(uint8_t inFirstByte, DictOperand &outOperand);

  private:
    IByteReaderWithPosition *mCFFFile;
    long long mInitialPosition;
    PDFHummus::EStatusCode mInternalState;
    uint8_t mCurrentOffsize;

    PDFHummus::EStatusCode Read3ByteUnsigned(unsigned long &outValue);
    PDFHummus::EStatusCode Read4ByteUnsigned(unsigned long &outValue);
    PDFHummus::EStatusCode Read4ByteSigned(long &outValue);
    PDFHummus::EStatusCode Read2ByteSigned(short &outValue);
    PDFHummus::EStatusCode ReadIntegerOperand(uint8_t inFirstByte, long &outValue);
    // make sure you get here after discarding the initial byte, 30.
    PDFHummus::EStatusCode ReadRealOperand(double &outValue, long &outRealValueFractalEnd);
};
