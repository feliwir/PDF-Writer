/*
   Source File : OpenTypeFileInput.h


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
#include "OpenTypePrimitiveReader.h"
#include "io/IByteReaderWithPosition.h"
#include "text/cff/CFFFileInput.h"

#include <list>
#include <map>
#include <string>

struct TableEntry
{
    unsigned long CheckSum;
    unsigned long Offset;
    unsigned long Length;
};

typedef std::map<unsigned long, TableEntry> ULongToTableEntryMap;

struct HeadTable
{
    double TableVersionNumber;
    double FontRevision;
    unsigned long CheckSumAdjustment;
    unsigned long MagicNumber;
    uint16_t Flags;
    uint16_t UnitsPerEm;
    long long Created;
    long long Modified;
    short XMin;
    short YMin;
    short XMax;
    short YMax;
    uint16_t MacStyle;
    uint16_t LowerRectPPEM;
    short FontDirectionHint;
    short IndexToLocFormat;
    short GlyphDataFormat;
};

struct MaxpTable
{
    double TableVersionNumber;
    uint16_t NumGlyphs;
    uint16_t MaxPoints;
    uint16_t MaxCountours;
    uint16_t MaxCompositePoints;
    uint16_t MaxCompositeContours;
    uint16_t MaxZones;
    uint16_t MaxTwilightPoints;
    uint16_t MaxStorage;
    uint16_t MaxFunctionDefs;
    uint16_t MaxInstructionDefs;
    uint16_t MaxStackElements;
    uint16_t MaxSizeOfInstructions;
    uint16_t MaxComponentElements;
    uint16_t MaxCompontentDepth;
};

struct HHeaTable
{
    double TableVersionNumber;
    short Ascender;
    short Descender;
    short LineGap;
    uint16_t AdvanceWidthMax;
    short MinLeftSideBearing;
    short MinRightSideBearing;
    short XMaxExtent;
    short CaretSlopeRise;
    short CaretSlopeRun;
    short CaretOffset;
    short MetricDataFormat;
    uint16_t NumberOfHMetrics;
};

struct HMtxTableEntry
{
    uint16_t AdvanceWidth;
    short LeftSideBearing;
};

typedef HMtxTableEntry *HMtxTable;

struct OS2Table
{
    uint16_t Version;
    short AvgCharWidth;
    uint16_t WeightClass;
    uint16_t WidthClass;
    uint16_t fsType;
    short SubscriptXSize;
    short SubscriptYSize;
    short SubscriptXOffset;
    short SubscriptYOffset;
    short SuperscriptXSize;
    short SuperscriptYSize;
    short SuperscriptXOffset;
    short SuperscriptYOffset;
    short StrikeoutSize;
    short StrikeoutPosition;
    short FamilyClass;
    uint8_t Panose[10];
    unsigned long UnicodeRange1;
    unsigned long UnicodeRange2;
    unsigned long UnicodeRange3;
    unsigned long UnicodeRange4;
    char AchVendID[4];
    uint16_t FSSelection;
    uint16_t FirstCharIndex;
    uint16_t LastCharIndex;
    short TypoAscender;
    short TypoDescender;
    short TypoLineGap;
    uint16_t WinAscent;
    uint16_t WinDescent;
    unsigned long CodePageRange1;
    unsigned long CodePageRange2;
    short XHeight;
    short CapHeight;
    uint16_t DefaultChar;
    uint16_t BreakChar;
    uint16_t MaxContext;
};

struct NameTableEntry
{
    uint16_t PlatformID;
    uint16_t EncodingID;
    uint16_t LanguageID;
    uint16_t NameID;
    uint16_t Length;
    uint16_t Offset;
    char *String;
};

struct NameTable
{
    uint16_t mNameEntriesCount;
    NameTableEntry *mNameEntries;
};

typedef unsigned long *LocaTable;
typedef std::list<unsigned long> ULongList;
typedef std::list<uint32_t> UIntList;

/* this time it's gonna be just what's intersting for my subsetting purposes - which is the
   dependencies ('n some other stuff) */
struct GlyphEntry
{
    short NumberOfContours;
    short XMin;
    short YMin;
    short XMax;
    short YMax;
    UIntList mComponentGlyphs; // will be empty for simple glyphs, and with component glyph indexes for components
};

typedef GlyphEntry **GlyfTable;

typedef std::map<uint16_t, GlyphEntry *> UShortToGlyphEntryMap;

enum EOpenTypeInputType
{
    EOpenTypeTrueType,
    EOpenTypeCFF
};

class OpenTypeFileInput
{
  public:
    OpenTypeFileInput(void);
    ~OpenTypeFileInput(void);

    charta::EStatusCode ReadOpenTypeFile(const std::string &inFontFilePath, uint16_t inFaceIndex);
    charta::EStatusCode ReadOpenTypeFile(IByteReaderWithPosition *inTrueTypeFile, uint16_t inFaceIndex);

    EOpenTypeInputType GetOpenTypeFontType();

    uint16_t GetGlyphsCount() const;

    unsigned long mHeaderOffset;
    unsigned long mTableOffset;

    uint16_t mFaceIndex;

    // read tables
    HeadTable mHead;
    MaxpTable mMaxp;
    HHeaTable mHHea;
    HMtxTable mHMtx;
    OS2Table mOS2;
    NameTable mName;
    LocaTable mLoca;
    GlyfTable mGlyf;

    // OS2 (surprise may not always exist. in dfonts for instance)
    bool mOS2Exists;

    // not read, but can tell if they are there
    bool mCVTExists;
    bool mFPGMExists;
    bool mPREPExists;

    TableEntry *GetTableEntry(const char *inTagName);

    CFFFileInput mCFF;

  private:
    OpenTypePrimitiveReader mPrimitivesReader;
    EOpenTypeInputType mFontType;
    uint16_t mTablesCount;
    ULongToTableEntryMap mTables;
    UShortToGlyphEntryMap mActualGlyphs; // using actual glyphs to map the glyphs that are not empty
                                         // (yeah, when parsing subset fonts...some glyphs might just
                                         // be empty, to avoid having to change the glyphs indices. some
                                         // technique some producers use

    charta::EStatusCode ReadOpenTypeHeader();
    charta::EStatusCode ReadOpenTypeSFNT();
    charta::EStatusCode ReadOpenTypeSFNTFromDfont();
    charta::EStatusCode ReadHead();
    charta::EStatusCode ReadMaxP();
    charta::EStatusCode ReadHHea();
    charta::EStatusCode ReadHMtx();
    charta::EStatusCode ReadOS2();
    charta::EStatusCode ReadName();
    charta::EStatusCode ReadLoca();
    charta::EStatusCode ReadGlyfForDependencies();
    unsigned long GetTag(const char *inTagName);
    void FreeTables();
    charta::EStatusCode ReadCFF();
};
