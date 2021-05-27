/*
   Source File : CFFFileInput.h


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

#include "CFFPrimitiveReader.h"
#include "EStatusCode.h"
#include "IType2InterpreterImplementation.h"
#include "io/IByteReaderWithPosition.h"

#include <string.h>

#include <list>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

struct CFFHeader
{
    uint8_t major;
    uint8_t minor;
    uint8_t hdrSize;
    uint8_t offSize;
};

typedef std::map<std::string, uint16_t> StringToUShort;
typedef std::pair<bool, uint16_t> BoolAndUShort;

typedef std::list<CharString *> CharStringList;

typedef CharString *CharStringsIndex;

// this time it's the font charstrings
struct CharStrings
{
    CharStrings()
    {
        mCharStringsIndex = NULL;
        mCharStringsType = 0;
        mCharStringsCount = 0;
    }

    uint8_t mCharStringsType;
    uint16_t mCharStringsCount;
    CharStringsIndex mCharStringsIndex;
};

enum ECharSetType
{
    eCharSetISOAdobe = 0,
    eCharSetExpert,
    eCharSetExpertSubset,
    eCharSetCustom
};

typedef std::map<uint16_t, CharString *> UShortToCharStringMap;

struct CharSetInfo
{
    CharSetInfo()
    {
        mSIDs = NULL;
    }

    ECharSetType mType;
    UShortToCharStringMap mSIDToGlyphMap;
    uint16_t *mSIDs; // count is like glyphs count
};

typedef std::vector<CharSetInfo *> CharSetInfoVector;

enum EEncodingType
{
    eEncodingStandard = 0,
    eEncodingExpert,
    eEncodingCustom
};

typedef std::list<uint8_t> ByteList;
typedef std::map<uint16_t, ByteList> UShortToByteList;

typedef std::pair<uint8_t, uint16_t> ByteAndUShort;

struct EncodingsInfo
{
    EncodingsInfo()
    {
        mEncoding = NULL;
    }

    long long mEncodingStart;
    long long mEncodingEnd;

    EEncodingType mType;
    uint8_t mEncodingsCount;
    uint8_t *mEncoding;
    UShortToByteList mSupplements;
};

struct PrivateDictInfo
{
    PrivateDictInfo()
    {
        mPrivateDictStart = 0;
        mPrivateDictEnd = 0;
        mLocalSubrs = NULL;
    }

    long long mPrivateDictStart;
    long long mPrivateDictEnd;
    UShortToDictOperandListMap mPrivateDict;
    CharStrings *mLocalSubrs;
};

typedef std::map<long long, CharStrings *> LongFilePositionTypeToCharStringsMap;

struct FontDictInfo
{
    long long mFontDictStart;
    long long mFontDictEnd;
    UShortToDictOperandListMap mFontDict;
    PrivateDictInfo mPrivateDict;
};

struct TopDictInfo
{
    TopDictInfo()
    {
        mFDArray = NULL;
        mFDSelect = NULL;
        mCharSet = NULL;
        mEncoding = NULL;
    }

    UShortToDictOperandListMap mTopDict;
    CharSetInfo *mCharSet;
    EncodingsInfo *mEncoding;
    FontDictInfo *mFDArray;
    FontDictInfo **mFDSelect; // size is like glyphsize. each cell references the relevant FontDict
};

typedef std::vector<EncodingsInfo *> EncodingsInfoVector;

struct StringLess
{
    bool operator()(const char *left, const char *right) const
    {
        return strcmp(left, right) < 0;
    }
};

typedef std::set<uint16_t> UShortSet;

struct CharString2Dependencies
{
    UShortSet mCharCodes;   // from seac-like endchar operator
    UShortSet mGlobalSubrs; // from callgsubr
    UShortSet mLocalSubrs;  // from callsubr
};

typedef std::map<const char *, uint16_t, StringLess> CharPToUShortMap;

class CFFFileInput : public Type2InterpreterImplementationAdapter
{
  public:
    CFFFileInput(void);
    ~CFFFileInput(void);

    // parses the whole CFF file, with all contained fonts
    charta::EStatusCode ReadCFFFile(IByteReaderWithPosition *inCFFFile);
    // parses the CFF file just for the particular font according to index. Index should be
    // according to how it appears in the CFF
    charta::EStatusCode ReadCFFFile(IByteReaderWithPosition *inCFFFile, uint16_t inFontIndex);
    // parses the CFF file just for the particular named font
    charta::EStatusCode ReadCFFFile(IByteReaderWithPosition *inCFFFile, const std::string &inFontName);

    // call only <i> after </i> calling the read method...got it?
    // calculate dependencies for a given charstring [it can be char, gsubr or localsubr].
    // the function is recursive, so that if the charstring has dependencies, then by the end of the
    // process their dependencies will be calculated too.
    // apart from the charstring you need to pass also the relevant localsubrs for this font, in case subrs
    // will be required. also the relevnat charstring and charset array should be passed in case
    // the seac flavor of endchar will be encountered.
    charta::EStatusCode CalculateDependenciesForCharIndex(uint16_t inFontIndex, uint16_t inCharStringIndex,
                                                          CharString2Dependencies &ioDependenciesInfo);

    uint16_t GetFontsCount(uint16_t inFontIndex) const;
    uint16_t GetCharStringsCount(uint16_t inFontIndex);
    std::string GetGlyphName(uint16_t inFontIndex, uint16_t inGlyphIndex);
    uint16_t GetGlyphSID(uint16_t inFontIndex, uint16_t inGlyphIndex);
    CharString *GetGlyphCharString(uint16_t inFontIndex, uint16_t inCharStringIndex);

    // use this wonderful little fellow when interpreting a charstring with
    // CharStringType2Intepreter. This will set the CFFFileInput with
    // the right items so that later you can use the IType2InterpreterImplementation
    // implementation here and avoid having to mess with passing gsubrs, lsubrs etc.
    // when interpreting just call these methods instead of yours to perform the relevant actions
    charta::EStatusCode PrepareForGlyphIntepretation(uint16_t inFontIndex, uint16_t inCharStringIndex);

    void Reset();

    // IType2InterpreterImplementation overrides
    virtual charta::EStatusCode ReadCharString(long long inCharStringStart, long long inCharStringEnd,
                                               uint8_t **outCharString);
    virtual CharString *GetLocalSubr(long inSubrIndex);
    virtual CharString *GetGlobalSubr(long inSubrIndex);
    virtual charta::EStatusCode Type2Endchar(const CharStringOperandList &inOperandList);

    // publicly available constructs

    // mCFFOffset should be added to any position here when referring to the beginning if the file containing this
    // segment. for instance, cff could be part of an OTF file definition, in which case the position is not 0.
    long long mCFFOffset;

    CFFHeader mHeader;
    uint16_t mFontsCount;
    std::list<std::string> mName;
    TopDictInfo *mTopDictIndex; // count is same as fonts count
    char **mStrings;
    uint16_t mStringsCount;
    long long mStringIndexPosition;
    long long mGlobalSubrsPosition;
    PrivateDictInfo *mPrivateDicts; // private dicts are the same as fonts count. refers to the topdict related private
                                    // dics, not to the FontDicts scenarios in CID.

  private:
    CharStrings mGlobalSubrs;
    CharStrings *mCharStrings;                        // count is same as fonts count
    LongFilePositionTypeToCharStringsMap mLocalSubrs; // count is NOT the same as fonts count [some may be shared, plus
                                                      // there might be more because of CID usage]
    CharSetInfoVector mCharSets; // count is NOT the same as fonts count [some charsets may be shared]. consult the top
                                 // dict charset pointer for the right charset
    EncodingsInfoVector mEncodings; // count is NOT the same as fonts count [some encodinds may be shared].

    CFFPrimitiveReader mPrimitivesReader;
    StringToUShort mNameToIndex;
    long long mNameIndexPosition;
    long long mTopDictIndexPosition;
    CharPToUShortMap mStringToSID;

    // for dependencies calculations using glyph interpretations. state.
    CharString2Dependencies *mCurrentDependencies;
    CharStrings *mCurrentLocalSubrs;
    CharStringList mAdditionalGlyphs;
    CharSetInfo *mCurrentCharsetInfo;

    std::string GetStringForSID(uint16_t inSID) const;
    charta::EStatusCode ReadHeader();
    charta::EStatusCode ReadNameIndex();
    charta::EStatusCode ReadIndexHeader(unsigned long **outOffsets, uint16_t &outItemsCount);
    charta::EStatusCode ReadTopDictIndex();
    charta::EStatusCode ReadDict(unsigned long inReadAmount, UShortToDictOperandListMap &outDict);
    charta::EStatusCode ReadStringIndex();
    charta::EStatusCode ReadGlobalSubrs();
    charta::EStatusCode ReadCharStrings();
    charta::EStatusCode ReadCharsets();
    charta::EStatusCode ReadEncodings();
    void FreeData();
    long long GetCharStringsPosition(uint16_t inFontIndex);
    long GetSingleIntegerValue(uint16_t inFontIndex, uint16_t inKey, long inDefault);
    charta::EStatusCode ReadSubrsFromIndex(uint16_t &outSubrsCount, CharStringsIndex *outSubrsIndex);
    long GetCharStringType(uint16_t inFontIndex);
    charta::EStatusCode ReadPrivateDicts();
    charta::EStatusCode ReadLocalSubrs();
    long GetSingleIntegerValueFromDict(const UShortToDictOperandListMap &inDict, uint16_t inKey, long inDefault);
    long long GetCharsetPosition(uint16_t inFontIndex);
    long long GetEncodingPosition(uint16_t inFontIndex);
    uint16_t GetBiasedIndex(uint16_t inSubroutineCollectionSize, long inSubroutineIndex);
    charta::EStatusCode ReadFormat0Charset(bool inIsCID, UShortToCharStringMap &ioCharMap, uint16_t **inSIDArray,
                                           const CharStrings &inCharStrings);
    charta::EStatusCode ReadFormat1Charset(bool inIsCID, UShortToCharStringMap &ioCharMap, uint16_t **inSIDArray,
                                           const CharStrings &inCharStrings);
    charta::EStatusCode ReadFormat2Charset(bool inIsCID, UShortToCharStringMap &ioCharMap, uint16_t **inSIDArray,
                                           const CharStrings &inCharStrings);
    void SetupSIDToGlyphMapWithStandard(const uint16_t *inStandardCharSet, uint16_t inStandardCharSetLength,
                                        UShortToCharStringMap &ioCharMap, const CharStrings &inCharStrings);
    CharString *GetCharacterFromStandardEncoding(uint8_t inCharacterCode);
    charta::EStatusCode ReadCIDInformation();
    charta::EStatusCode ReadFDArray(uint16_t inFontIndex);
    charta::EStatusCode ReadFDSelect(uint16_t inFontIndex);
    long long GetFDArrayPosition(uint16_t inFontIndex);
    charta::EStatusCode ReadPrivateDict(const UShortToDictOperandListMap &inReferencingDict,
                                        PrivateDictInfo *outPrivateDict);
    charta::EStatusCode ReadLocalSubrsForPrivateDict(PrivateDictInfo *inPrivateDict, uint8_t inCharStringType);
    long long GetFDSelectPosition(uint16_t inFontIndex);
    BoolAndUShort GetIndexForFontName(const std::string &inFontName);
    charta::EStatusCode ReadTopDictIndex(uint16_t inFontIndex);
    charta::EStatusCode ReadCharStrings(uint16_t inFontIndex);
    charta::EStatusCode ReadPrivateDicts(uint16_t inFontIndex);
    charta::EStatusCode ReadLocalSubrs(uint16_t inFontIndex);
    charta::EStatusCode ReadCharsets(uint16_t inFontIndex);
    void ReadEncoding(EncodingsInfo *inEncoding, long long inEncodingPosition);
    charta::EStatusCode ReadEncodings(uint16_t inFontIndex);
    charta::EStatusCode ReadCIDInformation(uint16_t inFontIndex);
    charta::EStatusCode ReadCFFFileByIndexOrName(IByteReaderWithPosition *inCFFFile, const std::string &inFontName,
                                                 uint16_t inFontIndex);
};
