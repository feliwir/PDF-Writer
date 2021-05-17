/*
   Source File : OpenTypeTest.cpp


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
#include "TestHelper.h"
#include "io/InputFile.h"
#include "text/opentype/OpenTypeFileInput.h"

#include "BoxingBase.h"
#include "io/IByteWriterWithPosition.h"
#include "io/OutputFile.h"
#include "text/cff/CharStringType2Tracer.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

EStatusCode SaveCharstringCode(unsigned short inFontIndex, unsigned short inGlyphIndex, CFFFileInput *inCFFFileInput)
{
    OutputFile glyphFile;

    EStatusCode status = glyphFile.OpenFile(
        RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, std::string("glyphCFF") + Long(inFontIndex).ToString() + "_" +
                                                         inCFFFileInput->GetGlyphName(0, inGlyphIndex) + ".txt"));
    if (status != PDFHummus::eSuccess)
        return status;

    CharStringType2Tracer tracer;

    status = tracer.TraceGlyphProgram(inFontIndex, inGlyphIndex, inCFFFileInput, glyphFile.GetOutputStream());

    glyphFile.CloseFile();

    return status;
}

TEST(OpenType, OpenType)
{
    EStatusCode status;
    InputFile otfFile;

    status = otfFile.OpenFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/BrushScriptStd.otf"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    OpenTypeFileInput openTypeReader;

    status = openTypeReader.ReadOpenTypeFile(otfFile.GetInputStream(), 0);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // show just abcd and notdef

    ASSERT_EQ(SaveCharstringCode(0, 0, &openTypeReader.mCFF), PDFHummus::eSuccess);
    for (unsigned short i = 66; i < 70; ++i)
        ASSERT_EQ(SaveCharstringCode(0, i, &openTypeReader.mCFF), PDFHummus::eSuccess);
}