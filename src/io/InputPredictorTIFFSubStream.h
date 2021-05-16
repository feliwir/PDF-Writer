/*
   Source File : InputPredictorTIFFSubStream.h


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

#include "IByteReader.h"

class InputPredictorTIFFSubStream : public IByteReader
{
  public:
    InputPredictorTIFFSubStream(void);
    virtual ~InputPredictorTIFFSubStream(void);

    // Takes ownership (use Assign(NULL,0,0,0) to unassign)
    InputPredictorTIFFSubStream(IByteReader *inSourceStream, size_t inColors, uint8_t inBitsPerComponent,
                                size_t inColumns);

    virtual size_t Read(uint8_t *inBuffer, size_t inBufferSize);

    virtual bool NotEnded();

    // Takes ownership (use Assign(NULL,0,0,0) to unassign)
    void Assign(IByteReader *inSourceStream, size_t inColors, uint8_t inBitsPerComponent, size_t inColumns);

  private:
    IByteReader *mSourceStream;
    size_t mColors;
    uint8_t mBitsPerComponent;
    size_t mColumns;

    uint8_t *mRowBuffer;
    size_t mReadColorsCount;
    unsigned short *mReadColors;
    unsigned short *mReadColorsIndex;
    uint8_t mIndexInColor;
    unsigned short mBitMask;

    void ReadByteFromColorsArray(uint8_t &outBuffer);
    void DecodeBufferToColors();
};
