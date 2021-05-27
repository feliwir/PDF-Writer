/*
   Source File : JPEGImageParser.h


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
class IByteReaderWithPosition;
struct JPEGImageInformation;

struct TwoLevelStatus
{
    EStatusCode primary;
    EStatusCode secondary;

    TwoLevelStatus(EStatusCode inPrimary, EStatusCode inSecondary)
    {
        primary = inPrimary;
        secondary = inSecondary;
    }

    bool eitherBad()
    {
        return primary != eSuccess || secondary != eSuccess;
    }
};

class JPEGImageParser
{
  public:
    JPEGImageParser(void);
    ~JPEGImageParser(void) = default;

    EStatusCode Parse(IByteReaderWithPosition *inImageStream, JPEGImageInformation &outImageInformation);

  private:
    IByteReaderWithPosition *mImageStream;
    uint8_t mReadBuffer[500];

    EStatusCode ReadJPEGID();
    EStatusCode ReadStreamToBuffer(unsigned long inAmountToRead);
    TwoLevelStatus ReadStreamToBuffer(unsigned long inAmountToRead, unsigned long &refReadLimit);
    EStatusCode ReadJpegTag(uint32_t &outTagID);
    EStatusCode ReadSOF0Data(JPEGImageInformation &outImageInformation);
    uint32_t GetIntValue(const uint8_t *inBuffer, bool inUseLittleEndian = false);
    void SkipStream(unsigned long inSkip);
    EStatusCode SkipStream(unsigned long inSkip, unsigned long &refReadLimit);
    EStatusCode ReadJFIFData(JPEGImageInformation &outImageInformation);
    EStatusCode ReadPhotoshopData(JPEGImageInformation &outImageInformation, bool outPhotoshopDataOK);
    EStatusCode ReadExifData(JPEGImageInformation &outImageInformation);
    EStatusCode GetResolutionFromExif(JPEGImageInformation &outImageInformation, unsigned long inXResolutionOffset,
                                      unsigned long inYResolutionOffset, unsigned long &inoutOffset,
                                      bool inUseLittleEndian);
    EStatusCode ReadRationalValue(double &outDoubleValue, bool inUseLittleEndian);
    EStatusCode ReadExifID();
    EStatusCode IsBigEndianExif(bool &outIsBigEndian);
    EStatusCode ReadIntValue(uint32_t &outIntValue, bool inUseLittleEndian = false);
    EStatusCode SkipTillChar(uint8_t inSkipUntilValue, unsigned long &refSkipLimit);
    EStatusCode ReadLongValue(unsigned long &outLongValue, bool inUseLittleEndian);
    TwoLevelStatus ReadLongValue(unsigned long &refReadLimit, unsigned long &outLongValue,
                                 bool inUseLittleEndian = false);
    unsigned long GetLongValue(const uint8_t *inBuffer, bool inUseLittleEndian);
    double GetFractValue(const uint8_t *inBuffer);
    EStatusCode SkipTag();
};
}; // namespace charta
