/*
   Source File : TrueTypePrimitiveWriter.h


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
#include <stdint.h>
#include <stdio.h>

namespace charta
{
class OutputStringBufferStream;
}

class TrueTypePrimitiveWriter
{
  public:
    TrueTypePrimitiveWriter(charta::OutputStringBufferStream *inTrueTypeFile = NULL);
    ~TrueTypePrimitiveWriter() = default;

    void SetOpenTypeStream(charta::OutputStringBufferStream *inTrueTypeFile);

    charta::EStatusCode GetInternalState();

    charta::EStatusCode WriteBYTE(uint8_t inValue);
    charta::EStatusCode WriteULONG(unsigned long inValue);
    charta::EStatusCode WriteUSHORT(uint16_t inValue);
    charta::EStatusCode WriteSHORT(short inValue);

    charta::EStatusCode Pad(int inCount);
    charta::EStatusCode PadTo4();

  private:
    charta::OutputStringBufferStream *mTrueTypeFile;
    charta::EStatusCode mInternalState;
};
