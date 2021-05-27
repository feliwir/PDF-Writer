/*
   Source File : TrueTypeEmbeddedFontWriter.h


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
#include "TrueTypePrimitiveWriter.h"
#include "io/InputFile.h"
#include "io/InputStringBufferStream.h"
#include "io/OutputStringBufferStream.h"
#include "text/opentype/OpenTypeFileInput.h"
#include "text/opentype/OpenTypePrimitiveReader.h"

#include <set>
#include <vector>

class FreeTypeFaceWrapper;
class ObjectsContext;

typedef std::vector<uint32_t> UIntVector;
typedef std::set<uint32_t> UIntSet;

class TrueTypeEmbeddedFontWriter
{
  public:
    TrueTypeEmbeddedFontWriter(void);
    ~TrueTypeEmbeddedFontWriter(void);

    charta::EStatusCode WriteEmbeddedFont(FreeTypeFaceWrapper &inFontInfo, const UIntVector &inSubsetGlyphIDs,
                                          ObjectsContext *inObjectsContext, ObjectIDType &outEmbeddedFontObjectID);

  private:
    OpenTypeFileInput mTrueTypeInput;
    InputFile mTrueTypeFile;
    OutputStringBufferStream mFontFileStream;
    TrueTypePrimitiveWriter mPrimitivesWriter;
    InputStringBufferStream mFontFileReaderStream; // now this might be confusing - i'm using a reader
                                                   // to my output buffer, so i can calculate checksums 'n such
    OpenTypePrimitiveReader mPrimitivesReader;     // same here...
    uint16_t mSubsetFontGlyphsCount;

    long long mCVTEntryWritingOffset;
    long long mFPGMEntryWritingOffset;
    long long mGLYFEntryWritingOffset;
    long long mHEADEntryWritingOffset;
    long long mHHEAEntryWritingOffset;
    long long mHMTXEntryWritingOffset;
    long long mLOCAEntryWritingOffset;
    long long mMAXPEntryWritingOffset;
    long long mPREPEntryWritingOffset;
    long long mNAMEEntryWritingOffset;
    long long mOS2EntryWritingOffset;
    long long mCMAPEntryWritingOffset;

    long long mHeadCheckSumOffset;

    charta::EStatusCode CreateTrueTypeSubset(FreeTypeFaceWrapper &inFontInfo, const UIntVector &inSubsetGlyphIDs,
                                             bool &outNotEmbedded, MyStringBuf &outFontProgram);

    void AddDependentGlyphs(UIntVector &ioSubsetGlyphIDs);
    bool AddComponentGlyphs(uint32_t inGlyphID, UIntSet &ioComponents);

    charta::EStatusCode WriteTrueTypeHeader();
    uint16_t GetSmallerPower2(uint16_t inNumber);
    unsigned long GetTag(const char *inTagName);
    void WriteEmptyTableEntry(const char *inTag, long long &outEntryPosition);
    charta::EStatusCode WriteHead();
    void WriteTableEntryData(long long inTableEntryOffset, long long inTableOffset, unsigned long inTableLength);
    charta::EStatusCode WriteHHea();
    charta::EStatusCode WriteHMtx();
    charta::EStatusCode WriteMaxp();
    charta::EStatusCode WriteCVT();
    charta::EStatusCode WriteFPGM();
    charta::EStatusCode WritePREP();
    charta::EStatusCode WriteNAME();
    charta::EStatusCode WriteOS2();
    charta::EStatusCode WriteGlyf(const UIntVector &inSubsetGlyphIDs, unsigned long *inLocaTable);
    charta::EStatusCode WriteLoca(unsigned long *inLocaTable);
    charta::EStatusCode WriteCMAP();
    unsigned long GetCheckSum(long long inOffset, unsigned long inLength);
    charta::EStatusCode CreateHeadTableCheckSumAdjustment();
    charta::EStatusCode CreateTableCopy(const char *inTableName, long long inTableEntryLocation);
};
