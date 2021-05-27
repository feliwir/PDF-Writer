/*
   Source File : OpenTypePrimitiveReader.h


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
#include "io/IByteReaderWithPosition.h"

class OpenTypePrimitiveReader
{
  public:
    OpenTypePrimitiveReader(IByteReaderWithPosition *inTrueTypeFile = NULL);
    ~OpenTypePrimitiveReader(void) = default;

    void SetOpenTypeStream(IByteReaderWithPosition *inOpenTypeFile);
    void SetOffset(long long inNewOffset);
    void Skip(size_t inToSkip);
    long long GetCurrentPosition();
    charta::EStatusCode GetInternalState();

    charta::EStatusCode ReadBYTE(unsigned char &outValue);
    charta::EStatusCode ReadCHAR(char &outValue);
    charta::EStatusCode ReadUSHORT(uint16_t &outValue);
    charta::EStatusCode ReadSHORT(short &outValue);
    charta::EStatusCode ReadULONG(unsigned long &outValue);
    charta::EStatusCode ReadLONG(long &outValue);
    charta::EStatusCode ReadLongDateTime(long long &outValue);
    charta::EStatusCode ReadFixed(double &outValue);
    charta::EStatusCode Read(uint8_t *inBuffer, size_t inBufferSize);

    IByteReaderWithPosition *GetReadStream();

  private:
    IByteReaderWithPosition *mOpenTypeFile;
    long long mInitialPosition;
    charta::EStatusCode mInternalState;
};
