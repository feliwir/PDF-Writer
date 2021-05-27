/*
   Source File : Type1ToCFFEmbeddedFontWriter.h


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
#include "MyStringBuf.h"
#include "ObjectsBasicTypes.h"
#include "Type1Input.h"
#include "io/InputFile.h"
#include "io/OutputStringBufferStream.h"
#include "text/cff/CFFPrimitiveWriter.h"

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

typedef std::vector<uint32_t> UIntVector;
typedef std::set<uint32_t> UIntSet;
typedef std::vector<std::string> StringVector;
typedef std::map<std::string, uint16_t> StringToUShortMap;
typedef std::pair<bool, uint16_t> BoolAndUShort;
typedef std::set<std::string> StringSet;

class FreeTypeFaceWrapper;
class ObjectsContext;

class Type1ToCFFEmbeddedFontWriter
{
  public:
    Type1ToCFFEmbeddedFontWriter(void);
    ~Type1ToCFFEmbeddedFontWriter(void);

    charta::EStatusCode WriteEmbeddedFont(FreeTypeFaceWrapper &inFontInfo, const UIntVector &inSubsetGlyphIDs,
                                          const std::string &inFontFile3SubType, const std::string &inSubsetFontName,
                                          ObjectsContext *inObjectsContext, ObjectIDType &outEmbeddedFontObjectID);

  private:
    Type1Input mType1Input;
    InputFile mType1File;
    CFFPrimitiveWriter mPrimitivesWriter;
    OutputStringBufferStream mFontFileStream;
    StringVector mStrings;
    StringToUShortMap mNonStandardStringToIndex;
    uint16_t *mCharset;

    // placeholders positions
    long long mCharsetPlaceHolderPosition;
    long long mEncodingPlaceHolderPosition;
    long long mCharstringsPlaceHolderPosition;
    long long mPrivatePlaceHolderPosition;

    long long mEncodingPosition;
    long long mCharsetPosition;
    long long mCharStringPosition;
    long long mPrivateSize;
    long long mPrivatePosition;

    charta::EStatusCode CreateCFFSubset(FreeTypeFaceWrapper &inFontInfo, const UIntVector &inSubsetGlyphIDs,
                                        const std::string &inSubsetFontName, bool &outNotEmbedded,
                                        MyStringBuf &outFontProgram);
    charta::EStatusCode AddDependentGlyphs(StringVector &ioSubsetGlyphIDs);
    charta::EStatusCode AddComponentGlyphs(const std::string &inGlyphID, StringSet &ioComponents,
                                           bool &outFoundComponents);
    charta::EStatusCode WriteCFFHeader();
    charta::EStatusCode WriteName(const std::string &inSubsetFontName);
    uint8_t GetMostCompressedOffsetSize(unsigned long inOffset);
    charta::EStatusCode WriteTopIndex();
    charta::EStatusCode WriteTopDictSegment(MyStringBuf &ioTopDictSegment);
    uint16_t AddStringToStringsArray(const std::string &inString);
    BoolAndUShort FindStandardString(const std::string &inStringToFind);
    void AddStringOperandIfNotEmpty(CFFPrimitiveWriter &inWriter, const std::string &inString, uint16_t inOperator);
    void AddNumberOperandIfNotDefault(CFFPrimitiveWriter &inWriter, int inOperand, uint16_t inOperator, int inDefault);
    void AddNumberOperandIfNotDefault(CFFPrimitiveWriter &inWriter, double inOperand, uint16_t inOperator,
                                      double inDefault);
    void AddDeltaVectorIfNotEmpty(CFFPrimitiveWriter &inWriter, const std::vector<int> &inArray, uint16_t inOperator);
    void AddDeltaVectorIfNotEmpty(CFFPrimitiveWriter &inWriter, const std::vector<double> &inArray,
                                  uint16_t inOperator);
    charta::EStatusCode WriteStringIndex();
    charta::EStatusCode WriteGlobalSubrsIndex();
    charta::EStatusCode WriteEncodings(const StringVector &inSubsetGlyphIDs);
    void FreeTemporaryStructs();
    void PrepareCharSetArray(const StringVector &inSubsetGlyphIDs);
    charta::EStatusCode WriteCharsets(const StringVector &inSubsetGlyphIDs);
    charta::EStatusCode WriteCharStrings(const StringVector &inSubsetGlyphIDs);
    charta::EStatusCode WritePrivateDictionary();
    charta::EStatusCode UpdateIndexesAtTopDict();
    void TranslateFromFreeTypeToType1(FreeTypeFaceWrapper &inFontInfo, const UIntVector &inSubsetGlyphIDs,
                                      StringVector &outGlyphNames);
};
