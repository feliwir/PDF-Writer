/*
   Source File : CIDFontWriter.h


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
#include "ObjectsBasicTypes.h"
#include "WrittenFontRepresentation.h"

#include <utility>
#include <vector>

class FreeTypeFaceWrapper;
class ObjectsContext;
class DictionaryContext;
class IDescendentFontWriter;
namespace charta
{
class IByteWriter;
}

typedef std::pair<uint32_t, GlyphEncodingInfo> UIntAndGlyphEncodingInfo;
typedef std::vector<UIntAndGlyphEncodingInfo> UIntAndGlyphEncodingInfoVector;

class CIDFontWriter
{
  public:
    CIDFontWriter(void);
    virtual ~CIDFontWriter(void);

    charta::EStatusCode WriteFont(FreeTypeFaceWrapper &inFontInfo, WrittenFontRepresentation *inFontOccurrence,
                                  ObjectsContext *inObjectsContext, IDescendentFontWriter *inDescendentFontWriter,
                                  bool inEmbedFont);

  private:
    FreeTypeFaceWrapper *mFontInfo;
    WrittenFontRepresentation *mFontOccurrence;
    ObjectsContext *mObjectsContext;
    UIntAndGlyphEncodingInfoVector mCharactersVector;

    void WriteEncoding(DictionaryContext *inFontContext);
    void CalculateCharacterEncodingArray();
    void WriteToUnicodeMap(ObjectIDType inToUnicodeMap);
    void WriteGlyphEntry(charta::IByteWriter *inWriter, uint16_t inEncodedCharacter,
                         const ULongVector &inUnicodeValues);
};
