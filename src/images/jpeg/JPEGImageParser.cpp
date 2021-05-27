/*
   Source File : JPEGImageParser.cpp


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
#include "images/jpeg/JPEGImageParser.h"
#include "images/jpeg/JPEGImageInformation.h"
#include "io/IByteReaderWithPosition.h"

#include <memory.h>

using namespace charta;

JPEGImageParser::JPEGImageParser() = default;

const uint32_t scSOF0TagID = 0xc0; // baseline format
const uint32_t scSOF1TagID = 0xc1;
const uint32_t scSOF2TagID = 0xc2;
const uint32_t scSOF3TagID = 0xc3;
const uint32_t scSOF5TagID = 0xc5;
const uint32_t scSOF6TagID = 0xc6;
const uint32_t scSOF7TagID = 0xc7;
const uint32_t scSOF9TagID = 0xc9;
const uint32_t scSOF10TagID = 0xca;
const uint32_t scSOF11TagID = 0xcb;
const uint32_t scSOF13TagID = 0xcd;
const uint32_t scSOF14TagID = 0xce;
const uint32_t scSOF15TagID = 0xcf;
const uint32_t scAPP0TagID = 0xe0;  // JFIF marker
const uint32_t scAPP1TagID = 0xe1;  // Exif marker
const uint32_t scAPP13TagID = 0xed; // Photoshop marker
const uint32_t scTagID = 0xff;
const unsigned char scJPEGID[2] = {0xff, 0xd8};
const unsigned char scAPP1ID_1[6] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};
const unsigned char scAPP1ID_2[6] = {0x45, 0x78, 0x69, 0x66, 0x00, 0xFF};
const unsigned char scEOS = '\0';
const unsigned char sc8Bim[4] = {'8', 'B', 'I', 'M'};
const unsigned char scResolutionBIMID[2] = {0x03, 0xed};
const uint32_t scAPP1BigEndian = 0x4d4d;
const uint32_t scAPP1LittleEndian = 0x4949;
const uint32_t scAPP1xResolutionTagID = 0x011a;
const uint32_t scAPP1yResolutionTagID = 0x011b;
const uint32_t scAPP1ResolutionUnitTagID = 0x0128;

EStatusCode JPEGImageParser::Parse(IByteReaderWithPosition *inImageStream, JPEGImageInformation &outImageInformation)
{
    EStatusCode status = charta::eFailure;
    uint32_t tagID;
    bool PhotoshopMarkerNotFound = true;
    bool JFIFMarkerNotFound = true;
    bool SOFMarkerNotFound = true;
    bool ExifMarkerNotFound = true;

    mImageStream = inImageStream;

    do
    {
        status = ReadJPEGID();
        if (status != charta::eSuccess)
            break;

        do
        {
            if (!mImageStream->NotEnded())
                break;
            status = ReadJpegTag(tagID);
            if (status != charta::eSuccess)
                break;
            switch (tagID)
            {
            case scSOF0TagID:
            case scSOF1TagID:
            case scSOF2TagID:
            case scSOF3TagID:
            case scSOF5TagID:
            case scSOF6TagID:
            case scSOF7TagID:
            case scSOF9TagID:
            case scSOF10TagID:
            case scSOF11TagID:
            case scSOF13TagID:
            case scSOF14TagID:
            case scSOF15TagID:
                SOFMarkerNotFound = false;
                status = ReadSOF0Data(outImageInformation);
                break;
            case scAPP0TagID:
                if (JFIFMarkerNotFound)
                {
                    JFIFMarkerNotFound = false;
                    status = ReadJFIFData(outImageInformation);
                }
                else
                    SkipTag();
                break;
            case scAPP13TagID:
                if (PhotoshopMarkerNotFound)
                {
                    // photoshop tags may be corrupt, so internal method will return if the
                    // photoshop tag is OK. otherwise skip it, and wait for the next one...parhaps will be better
                    status = ReadPhotoshopData(outImageInformation, PhotoshopMarkerNotFound);
                }
                else
                    status = SkipTag();
                break;
            case scAPP1TagID:
                if (ExifMarkerNotFound)
                {
                    ExifMarkerNotFound = false;
                    status = ReadExifData(outImageInformation);
                    if (status != eSuccess)
                    {
                        // if unable to read marker it's either XMP or unsupported version of Exif. simply ignore
                        ExifMarkerNotFound = true;
                        status = eSuccess;
                    }
                }
                else
                    status = SkipTag();
                break;
            default:
                status = SkipTag();
                break;
            }
        } while ((ExifMarkerNotFound || PhotoshopMarkerNotFound || JFIFMarkerNotFound || SOFMarkerNotFound) &&
                 (0 == status));

        if (SOFMarkerNotFound)
        {
            status = charta::eFailure;
            break;
        }
        status = charta::eSuccess;
    } while (false);

    return status;
}

EStatusCode JPEGImageParser::ReadJPEGID()
{
    EStatusCode status = ReadStreamToBuffer(2);

    if (status != charta::eSuccess)
        return status;

    if (memcmp(mReadBuffer, scJPEGID, 2) != 0)
        return charta::eFailure;

    return charta::eSuccess;
}

EStatusCode JPEGImageParser::ReadStreamToBuffer(unsigned long inAmountToRead)
{
    if (inAmountToRead == mImageStream->Read(mReadBuffer, inAmountToRead))
        return charta::eSuccess;
    return charta::eFailure;
}

EStatusCode JPEGImageParser::ReadJpegTag(uint32_t &outTagID)
{
    EStatusCode status = ReadStreamToBuffer(2);

    if (charta::eSuccess == status)
    {
        if (scTagID == (uint32_t)mReadBuffer[0])
            outTagID = (uint32_t)mReadBuffer[1];
        else
            status = charta::eFailure;
    }
    return status;
}

EStatusCode JPEGImageParser::ReadSOF0Data(JPEGImageInformation &outImageInformation)
{
    uint32_t toSkip;
    EStatusCode status;

    status = ReadStreamToBuffer(8);
    if (charta::eSuccess == status)
    {
        toSkip = GetIntValue(mReadBuffer) - 8;
        outImageInformation.SamplesHeight = GetIntValue(mReadBuffer + 3);
        outImageInformation.SamplesWidth = GetIntValue(mReadBuffer + 5);
        outImageInformation.ColorComponentsCount = (uint32_t)mReadBuffer[7];
        SkipStream(toSkip);
    }
    return status;
}

uint32_t JPEGImageParser::GetIntValue(const uint8_t *inBuffer, bool inUseLittleEndian)
{
    uint32_t value;

    if (inUseLittleEndian)
    {
        value = (uint32_t)inBuffer[0];
        value += 0x100 * (uint32_t)inBuffer[1];
    }
    else
    {
        value = (uint32_t)inBuffer[1];
        value += 0x100 * (uint32_t)inBuffer[0];
    }

    return value;
}

void JPEGImageParser::SkipStream(unsigned long inSkip)
{
    mImageStream->Skip(inSkip);
}

EStatusCode JPEGImageParser::ReadJFIFData(JPEGImageInformation &outImageInformation)
{
    uint32_t toSkip;
    EStatusCode status;

    status = ReadStreamToBuffer(14);
    if (charta::eSuccess == status)
    {
        outImageInformation.JFIFInformationExists = true;
        toSkip = GetIntValue(mReadBuffer) - 14;
        outImageInformation.JFIFUnit = (uint32_t)mReadBuffer[9];
        outImageInformation.JFIFXDensity = GetIntValue(mReadBuffer + 10);
        outImageInformation.JFIFYDensity = GetIntValue(mReadBuffer + 12);
        SkipStream(toSkip);
    }
    return status;
}

TwoLevelStatus JPEGImageParser::ReadStreamToBuffer(unsigned long inAmountToRead, unsigned long &refReadLimit)
{
    if (refReadLimit < inAmountToRead)
        return TwoLevelStatus(charta::eSuccess, charta::eFailure);
    EStatusCode status = ReadStreamToBuffer(inAmountToRead);
    if (status == charta::eSuccess)
        refReadLimit -= inAmountToRead;
    return TwoLevelStatus(status, charta::eSuccess);
}
TwoLevelStatus JPEGImageParser::ReadLongValue(unsigned long &refReadLimit, unsigned long &outLongValue,
                                              bool inUseLittleEndian)
{
    if (refReadLimit < 4)
        return TwoLevelStatus(charta::eSuccess, charta::eFailure);

    EStatusCode status = ReadLongValue(outLongValue, inUseLittleEndian);
    if (status == charta::eSuccess)
        refReadLimit -= 4;
    return TwoLevelStatus(status, charta::eSuccess);
}

EStatusCode JPEGImageParser::SkipStream(unsigned long inSkip, unsigned long &refReadLimit)
{
    if (refReadLimit < inSkip)
        return charta::eFailure;
    SkipStream(inSkip);
    refReadLimit -= inSkip;
    return charta::eSuccess;
}

EStatusCode JPEGImageParser::ReadPhotoshopData(JPEGImageInformation &outImageInformation, bool outPhotoshopDataOK)
{
    // code below uses a two level status where the primary is in charge of read error
    // and the seconary is in charge of realizing whether the data is correct.
    // error in the former should cause complete break. error in the latter is fine on the read level
    // and simply means the data is logically corrupt and should simply be skipped

    TwoLevelStatus twoLevelStatus(eSuccess, eSuccess);
    uint32_t intSkip;
    unsigned long toSkip;
    uint32_t nameSkip;
    unsigned long dataLength;
    bool resolutionBimNotFound = true;

    do
    {
        twoLevelStatus.primary = ReadIntValue(intSkip);
        if (twoLevelStatus.primary != charta::eSuccess)
            break;
        toSkip = intSkip - 2;
        twoLevelStatus.primary = SkipTillChar(scEOS, toSkip);
        if (twoLevelStatus.primary != charta::eSuccess)
            break;

        while (toSkip > 0 && resolutionBimNotFound)
        {
            twoLevelStatus = ReadStreamToBuffer(4, toSkip);
            if (twoLevelStatus.eitherBad())
                break;
            if (0 != memcmp(mReadBuffer, sc8Bim, 4))
                break;
            twoLevelStatus = ReadStreamToBuffer(3, toSkip);
            if (twoLevelStatus.eitherBad())
                break;
            nameSkip = (int)mReadBuffer[2];
            if (nameSkip % 2 == 0)
                ++nameSkip;
            twoLevelStatus.secondary = SkipStream(nameSkip, toSkip);
            if (twoLevelStatus.secondary != 0)
                break;
            resolutionBimNotFound = (0 != memcmp(mReadBuffer, scResolutionBIMID, 2));
            twoLevelStatus = ReadLongValue(toSkip, dataLength);
            if (twoLevelStatus.eitherBad())
                break;
            if (resolutionBimNotFound)
            {
                if (dataLength % 2 == 1)
                    ++dataLength;
                twoLevelStatus.secondary = SkipStream(dataLength, toSkip);
                if (twoLevelStatus.secondary != charta::eSuccess)
                    break;
            }
            else
            {
                twoLevelStatus = ReadStreamToBuffer(16, toSkip);
                if (twoLevelStatus.eitherBad())
                    break;

                outImageInformation.PhotoshopInformationExists = true;
                outImageInformation.PhotoshopXDensity = GetIntValue(mReadBuffer) + GetFractValue(mReadBuffer + 2);
                outImageInformation.PhotoshopYDensity = GetIntValue(mReadBuffer + 8) + GetFractValue(mReadBuffer + 10);
            }
        }

        if (charta::eSuccess == twoLevelStatus.primary)
            SkipStream(toSkip);
    } while (false);

    outPhotoshopDataOK = !resolutionBimNotFound && twoLevelStatus.secondary == charta::eSuccess;
    return twoLevelStatus.primary;
}

EStatusCode JPEGImageParser::ReadExifData(JPEGImageInformation &outImageInformation)
{
    EStatusCode status;
    unsigned long ifdOffset;
    uint32_t ifdDirectorySize, tagID, toSkip;
    bool isBigEndian;
    unsigned long xResolutionOffset = 0;
    unsigned long yResolutionOffset = 0;
    uint32_t resolutionUnitValue = 0;

    do
    {
        // read Exif Tag size
        status = ReadIntValue(toSkip);
        if (status != charta::eSuccess)
            break;

        toSkip -= 2;

        // read Exif ID
        status = ReadExifID();
        toSkip -= 6;
        if (status != charta::eSuccess)
        {
            // might be wrong ID
            SkipStream(toSkip);
            break;
        }

        // read encoding
        status = IsBigEndianExif(isBigEndian);
        if (status != charta::eSuccess)
            break;

        toSkip -= 2;

        // skip 0x002a
        SkipStream(2);
        toSkip -= 2;

        // read IFD0 offset
        status = ReadLongValue(ifdOffset, !isBigEndian);
        if (status != charta::eSuccess)
            break;

        toSkip -= 4;

        // skip to the IFD beginning
        SkipStream(ifdOffset - 8);
        toSkip -= (ifdOffset - 8);

        // read IFD size
        status = ReadIntValue(ifdDirectorySize, !isBigEndian);
        if (status != charta::eSuccess)
            break;

        toSkip -= 2;

        for (uint32_t i = 0; i < ifdDirectorySize; i++)
        {
            if (0 != xResolutionOffset && 0 != yResolutionOffset && 0 != resolutionUnitValue)
            {
                SkipStream(12 * (ifdDirectorySize - i));
                toSkip -= (12 * (ifdDirectorySize - i));
                break;
            }

            status = ReadIntValue(tagID, !isBigEndian);
            if (status != charta::eSuccess)
                break;

            toSkip -= 2;

            switch (tagID)
            {
            case scAPP1xResolutionTagID:
                SkipStream(6);
                status = ReadLongValue(xResolutionOffset, !isBigEndian);
                break;
            case scAPP1yResolutionTagID:
                SkipStream(6);
                status = ReadLongValue(yResolutionOffset, !isBigEndian);
                break;
            case scAPP1ResolutionUnitTagID:
                SkipStream(6);
                status = ReadIntValue(resolutionUnitValue, !isBigEndian);
                SkipStream(2);
                break;
            default:
                SkipStream(10);
                break;
            }

            toSkip -= 10;
            if (status != charta::eSuccess)
                break;
        }

        outImageInformation.ExifInformationExists = true;
        if (resolutionUnitValue != 0)
            outImageInformation.ExifUnit = resolutionUnitValue;
        else
            outImageInformation.ExifUnit = 2;

        unsigned long currentOffset = 0;
        if (ifdOffset > 8)
        {
            // that would be the case where the IFD data appears before thee ifd header. avoid issues with negative skip
            // values by placing the position before the table
            mImageStream->SetPosition(mImageStream->GetCurrentPosition() - (ifdOffset + ifdDirectorySize * 12 + 2));
            toSkip += (ifdOffset + ifdDirectorySize * 12 + 2);
        }
        else
        {
            currentOffset = ifdOffset + ifdDirectorySize * 12 + 2;
        }
        unsigned long tempOffset = currentOffset;
        status =
            GetResolutionFromExif(outImageInformation, xResolutionOffset, yResolutionOffset, tempOffset, !isBigEndian);
        if (status != charta::eSuccess)
            break;

        toSkip -= (tempOffset - currentOffset);

        if (charta::eSuccess == status)
            SkipStream(toSkip);
    } while (false);
    return status;
}

EStatusCode JPEGImageParser::GetResolutionFromExif(JPEGImageInformation &outImageInformation,
                                                   unsigned long inXResolutionOffset, unsigned long inYResolutionOffset,
                                                   unsigned long &inoutOffset, bool inUseLittleEndian)
{
    unsigned long firstOffset = 0, secondOffset = 0;
    bool xResolutionIsFirst = true;
    EStatusCode status = charta::eSuccess;

    outImageInformation.ExifXDensity = 0;
    outImageInformation.ExifYDensity = 0;

    if (inXResolutionOffset != 0 && inXResolutionOffset < inYResolutionOffset)
    {
        firstOffset = inXResolutionOffset;

        if (inYResolutionOffset != 0)
            secondOffset = inYResolutionOffset;
    }
    else if (inYResolutionOffset != 0)
    {
        firstOffset = inYResolutionOffset;
        xResolutionIsFirst = false;

        if (inXResolutionOffset != 0)
            secondOffset = inXResolutionOffset;
    }

    do
    {
        if (0 == firstOffset)
            break;

        SkipStream(firstOffset - inoutOffset);
        inoutOffset += (firstOffset - inoutOffset);

        status =
            ReadRationalValue(xResolutionIsFirst ? outImageInformation.ExifXDensity : outImageInformation.ExifYDensity,
                              inUseLittleEndian);

        if (status != charta::eSuccess)
            break;

        inoutOffset += 8;

        if (0 == secondOffset)
            break;

        SkipStream(secondOffset - firstOffset - 8);
        inoutOffset += (secondOffset - firstOffset - 8);

        status =
            ReadRationalValue(xResolutionIsFirst ? outImageInformation.ExifYDensity : outImageInformation.ExifXDensity,
                              inUseLittleEndian);
        if (status != charta::eSuccess)
            break;

        inoutOffset += 8;
    } while (false);
    return status;
}

EStatusCode JPEGImageParser::ReadRationalValue(double &outDoubleValue, bool inUseLittleEndian)
{
    unsigned long numerator, denominator;
    EStatusCode status = ReadLongValue(numerator, inUseLittleEndian);

    if (status != charta::eSuccess)
        return status;

    status = ReadLongValue(denominator, inUseLittleEndian);

    if (status != charta::eSuccess)
        return status;

    outDoubleValue = ((double)numerator) / ((double)denominator);
    return status;
}

EStatusCode JPEGImageParser::ReadExifID()
{
    EStatusCode status = ReadStreamToBuffer(6);
    if (status != charta::eSuccess)
        return status;

    if (memcmp(mReadBuffer, scAPP1ID_1, 6) != 0 && memcmp(mReadBuffer, scAPP1ID_2, 6) != 0)
        return charta::eFailure;

    return charta::eSuccess;
}

EStatusCode JPEGImageParser::IsBigEndianExif(bool &outIsBigEndian)
{
    uint32_t encodingType;
    EStatusCode status = ReadIntValue(encodingType);

    if (status != charta::eSuccess)
        return status;

    if (encodingType == scAPP1BigEndian)
        outIsBigEndian = true;
    else if (encodingType == scAPP1LittleEndian)
        outIsBigEndian = false;
    else
        return charta::eFailure;

    return charta::eSuccess;
}

EStatusCode JPEGImageParser::ReadIntValue(uint32_t &outIntValue, bool inUseLittleEndian)
{
    EStatusCode status = ReadStreamToBuffer(2);

    if (charta::eSuccess == status)
        outIntValue = GetIntValue(mReadBuffer, inUseLittleEndian);
    return status;
}

EStatusCode JPEGImageParser::SkipTillChar(uint8_t inSkipUntilValue, unsigned long &refSkipLimit)
{
    EStatusCode status = charta::eSuccess;
    bool charNotFound = true;

    while (charNotFound && (charta::eSuccess == status) && (refSkipLimit > 0))
    {
        status = ReadStreamToBuffer(1);
        if (charta::eSuccess == status)
        {
            --refSkipLimit;
            if (mReadBuffer[0] == inSkipUntilValue)
                charNotFound = false;
        }
    }
    return status;
}

EStatusCode JPEGImageParser::ReadLongValue(unsigned long &outLongValue, bool inUseLittleEndian)
{
    EStatusCode status = ReadStreamToBuffer(4);

    if (charta::eSuccess == status)
        outLongValue = GetLongValue(mReadBuffer, inUseLittleEndian);
    return status;
}

unsigned long JPEGImageParser::GetLongValue(const uint8_t *inBuffer, bool inUseLittleEndian)
{
    unsigned long value;

    if (inUseLittleEndian)
    {
        value = (uint32_t)inBuffer[0];
        value += 0x100 * (uint32_t)inBuffer[1];
        value += 0x10000 * (uint32_t)inBuffer[2];
        value += 0x1000000 * (uint32_t)inBuffer[3];
    }
    else
    {
        value = (uint32_t)inBuffer[3];
        value += 0x100 * (uint32_t)inBuffer[2];
        value += 0x10000 * (uint32_t)inBuffer[1];
        value += 0x1000000 * (uint32_t)inBuffer[0];
    }
    return value;
}

double JPEGImageParser::GetFractValue(const uint8_t *inBuffer)
{
    double value;

    value = (double)inBuffer[0] / (double)0x100 + (double)inBuffer[1] / (double)0x10;
    return value;
}

EStatusCode JPEGImageParser::SkipTag()
{
    EStatusCode status;
    uint32_t toSkip;

    status = ReadIntValue(toSkip);
    // skipping -2 because int was already read
    if (charta::eSuccess == status)
        SkipStream(toSkip - 2);
    return status;
}
