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

struct JPEGImageInformation;
namespace charta
{
class IByteReaderWithPosition;
};

struct TwoLevelStatus
{
    charta::EStatusCode primary;
    charta::EStatusCode secondary;

    TwoLevelStatus(charta::EStatusCode inPrimary, charta::EStatusCode inSecondary)
    {
        primary = inPrimary;
        secondary = inSecondary;
    }

    bool eitherBad()
    {
        return primary != charta::eSuccess || secondary != charta::eSuccess;
    }
};

class JPEGImageParser
{
  public:
    JPEGImageParser(void);
    ~JPEGImageParser(void) = default;

    charta::EStatusCode Parse(charta::IByteReaderWithPosition *inImageStream,
                              JPEGImageInformation &outImageInformation);

  private:
    charta::IByteReaderWithPosition *mImageStream;
    uint8_t mReadBuffer[500];

    charta::EStatusCode ReadJPEGID();
    charta::EStatusCode ReadStreamToBuffer(unsigned long inAmountToRead);
    TwoLevelStatus ReadStreamToBuffer(unsigned long inAmountToRead, unsigned long &refReadLimit);
    charta::EStatusCode ReadJpegTag(uint32_t &outTagID);
    charta::EStatusCode ReadSOF0Data(JPEGImageInformation &outImageInformation);
    uint32_t GetIntValue(const uint8_t *inBuffer, bool inUseLittleEndian = false);
    void SkipStream(unsigned long inSkip);
    charta::EStatusCode SkipStream(unsigned long inSkip, unsigned long &refReadLimit);
    charta::EStatusCode ReadJFIFData(JPEGImageInformation &outImageInformation);
    charta::EStatusCode ReadPhotoshopData(JPEGImageInformation &outImageInformation, bool outPhotoshopDataOK);
    charta::EStatusCode ReadExifData(JPEGImageInformation &outImageInformation);
    charta::EStatusCode GetResolutionFromExif(JPEGImageInformation &outImageInformation,
                                              unsigned long inXResolutionOffset, unsigned long inYResolutionOffset,
                                              unsigned long &inoutOffset, bool inUseLittleEndian);
    charta::EStatusCode ReadRationalValue(double &outDoubleValue, bool inUseLittleEndian);
    charta::EStatusCode ReadExifID();
    charta::EStatusCode IsBigEndianExif(bool &outIsBigEndian);
    charta::EStatusCode ReadIntValue(uint32_t &outIntValue, bool inUseLittleEndian = false);
    charta::EStatusCode SkipTillChar(uint8_t inSkipUntilValue, unsigned long &refSkipLimit);
    charta::EStatusCode ReadLongValue(unsigned long &outLongValue, bool inUseLittleEndian);
    TwoLevelStatus ReadLongValue(unsigned long &refReadLimit, unsigned long &outLongValue,
                                 bool inUseLittleEndian = false);
    unsigned long GetLongValue(const uint8_t *inBuffer, bool inUseLittleEndian);
    double GetFractValue(const uint8_t *inBuffer);
    charta::EStatusCode SkipTag();
};
