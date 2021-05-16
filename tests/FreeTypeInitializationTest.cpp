/*
   Source File : FreeTypeInitializationTest.cpp


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
#include "text/freetype/EFontStretch.h"
#include "text/freetype/FreeTypeFaceWrapper.h"
#include "text/freetype/FreeTypeWrapper.h"

#include "Singleton.h"
#include "TestHelper.h"
#include "Trace.h"

#include FT_XFREE86_H
#include FT_CID_H
#include FT_TYPE1_TABLES_H
#include FT_TRUETYPE_TAGS_H
#include FT_TRUETYPE_TABLES_H

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

EStatusCode ShowGlobalFontProperties(FreeTypeWrapper & /*inFreeType*/, FT_Face inFace)
{
    EStatusCode status = PDFHummus::eSuccess;

    FreeTypeFaceWrapper face(inFace, "", 0, false);

    std::cout << "Font Family = " << (face->family_name != nullptr ? face->family_name : "somefont") << "\n";
    std::cout << "Font Style = " << (face->style_name != nullptr ? face->style_name : "somestyle") << "\n";
    std::cout << "Font Format = " << FT_Get_X11_Font_Format(face) << "\n";
    std::cout << "Font CID Keyed (does not includes sfnts) = "
              << ((face->face_flags & FT_FACE_FLAG_CID_KEYED) != 0 ? "yes" : "no") << "\n";
    std::cout << "Font SFNT modeled = " << (FT_IS_SFNT(face) ? "yes" : "no") << "\n";
    if (FT_IS_SFNT(face))
    {
        FT_ULong length = 0;
        if (FT_Load_Sfnt_Table(face, TTAG_CFF, 0, nullptr, &length) == 0)
            std::cout << "CCF table found, length is " << length << "\n";
    }

    FT_Bool isCID;

    if (FT_Get_CID_Is_Internally_CID_Keyed(face, &isCID) != 0)
    {
        std::cout << "No CID information to read\n";
        isCID = 0u;
    }
    else
        std::cout << "Font Internally CID (checks also sfnts) = " << (isCID != 0u ? "yes" : "no") << "\n";

    if (isCID != 0u)
    {
        const char *registry;
        const char *ordering;
        FT_Int supplement;
        if (FT_Get_CID_Registry_Ordering_Supplement(face, &registry, &ordering, &supplement) != 0)
        {
            std::cout << "Failed to read registry, ordering and supplement informaiton\n";
            status = PDFHummus::eFailure;
        }
        std::cout << "CID Registry = " << registry << "\n";
        std::cout << "CID Ordering = " << ordering << "\n";
        std::cout << "CID Supplement = " << supplement << "\n";
    }

    std::cout << "Font BBox = [" << face->bbox.xMin << " " << face->bbox.yMin << " " << face->bbox.xMax << " "
              << face->bbox.yMax << "]\n";
    std::cout << "Ascent " << face->ascender << "\n";
    std::cout << "Descent " << face->descender << "\n";
    std::cout << "Italic Angle = " << face.GetItalicAngle() << "\n";
    BoolAndFTShort capHeightResult = face.GetCapHeight();
    if (capHeightResult.first)
        std::cout << "Cap Height = " << capHeightResult.second << "\n";
    else
        std::cout << "No Cap Height value\n";
    BoolAndFTShort xHeightResult = face.GetxHeight();
    if (capHeightResult.first)
        std::cout << "x Height = " << xHeightResult.second << "\n";
    else
        std::cout << "No x Height value\n";
    std::cout << "StemV = " << face.GetStemV() << "\n";
    EFontStretch fontStretch = face.GetFontStretch();
    if (eFontStretchUknown == fontStretch)
        std::cout << "Unkown Stretch";
    else
        std::cout << "Stretch = " << fontStretch << "\n";

    FT_UShort fontWeight = face.GetFontWeight();
    if (1000 == fontWeight)
        std::cout << "Unknown Weight";
    else
        std::cout << "Weight = " << fontWeight << "\n";
    std::cout << "FontFlags = " << face.GetFontFlags() << "\n";
    if (FT_HAS_GLYPH_NAMES(face))
    {
        FT_UInt glyphIndex = FT_Get_Char_Index(face, 0x31);

        std::cout << "Font has glyph names\n";
        if (glyphIndex != 0)
        {
            char buffer[31];
            if (FT_Get_Glyph_Name(face, glyphIndex, buffer, 31) == 0)
                std::cout << "Glyph name for the 1 = " << buffer << "\n";
        }
    }
    else
    {
        std::cout << "Font does not have glyph names\n";
    }
    return status;
}

EStatusCode ShowFaceProperties(FreeTypeWrapper &inFreeType, const std::string &inFontFilePath,
                               const std::string &inSecondaryFontFilePath = "")
{
    FT_Face face;
    EStatusCode status = PDFHummus::eSuccess;

    if (inSecondaryFontFilePath.length() > 0)
        face = inFreeType.NewFace(inFontFilePath, inSecondaryFontFilePath, 0);
    else
        face = inFreeType.NewFace(inFontFilePath, 0);
    if (face == nullptr)
    {
        return PDFHummus::eFailure;
    }
    status = ShowGlobalFontProperties(inFreeType, face);

    inFreeType.DoneFace(face);
    return status;
}

TEST(FreeType, FreeTypeInitialization)
{
    Singleton<Trace>::GetInstance()->SetLogSettings(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "FreeTypeTest.txt"),
                                                    true, true);

    EStatusCode status = PDFHummus::eSuccess;
    FreeTypeWrapper ftWrapper;

    status = ShowFaceProperties(ftWrapper, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/arial.ttf"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = ShowFaceProperties(ftWrapper, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/HLB_____.PFB"),
                                RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/HLB_____.PFM"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status =
        ShowFaceProperties(ftWrapper, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/BrushScriptStd.otf"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    Singleton<Trace>::Reset();
}
