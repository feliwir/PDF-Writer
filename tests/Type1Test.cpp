/*
   Source File : Type1Test.cpp


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
#include "PSBool.h"
#include "TestHelper.h"
#include "io/IByteWriterWithPosition.h"
#include "io/InputFile.h"
#include "io/OutputFile.h"
#include "text/type1/CharStringType1Tracer.h"
#include "text/type1/Type1Input.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

void ShowIntVector(const std::vector<int> &inVector)
{
    auto it = inVector.begin();

    std::cout << "[ ";
    for (; it != inVector.end(); ++it)
        std::cout << *it << " ";
    std::cout << "]";
}

void ShowDoubleVector(const std::vector<double> &inVector)
{
    auto it = inVector.begin();

    std::cout << "[ ";
    for (; it != inVector.end(); ++it)
        std::cout << Double(*it).ToString().c_str() << " ";
    std::cout << "]";
}

void ShowDoubleArray(const double inDoubleArray[], int inSize)
{
    std::cout << "[ ";
    for (int i = 0; i < inSize; ++i)
        std::cout << Double(inDoubleArray[i]) << " ";
    std::cout << "]";
}

void ShowFontDictionary(const Type1FontDictionary &inFontDictionary)
{
    std::cout << "\nShowing Font Dictionary:\n"
              << "FontName = " << inFontDictionary.FontName.c_str() << "\n"
              << "PaintType = " << inFontDictionary.PaintType << "\n"
              << "FontType = " << inFontDictionary.FontType << "\n"
              << "FontBBox = ";
    ShowDoubleArray(inFontDictionary.FontBBox, 4);
    std::cout << "\nFontMatrix = ";
    ShowDoubleArray(inFontDictionary.FontMatrix, 6);
    std::cout << "\nUniqueID = " << inFontDictionary.UniqueID << "\n"
              << "StrokeWidth = " << inFontDictionary.StrokeWidth << "\n";
}

void ShowFontInfoDictionary(const Type1FontInfoDictionary &inFontInfoDictionary)
{
    std::cout << "\nShowing Font Info Dictionary:\n"
              << "version = " << inFontInfoDictionary.version.c_str() << "\n"
              << "Notice = " << inFontInfoDictionary.Notice.c_str() << "\n"
              << "Copyright = " << inFontInfoDictionary.Copyright.c_str() << "\n"
              << "FullName = " << inFontInfoDictionary.FullName.c_str() << "\n"
              << "FamilyName = " << inFontInfoDictionary.FamilyName.c_str() << "\n"
              << "Weight = " << inFontInfoDictionary.Weight.c_str() << "\n"
              << "ItalicAngle = " << Double(inFontInfoDictionary.ItalicAngle).ToString().c_str() << "\n"
              << "isFixedPitch = " << PSBool(inFontInfoDictionary.isFixedPitch).ToString().c_str() << "\n"
              << "UnderlinePosition = " << Double(inFontInfoDictionary.UnderlinePosition).ToString().c_str() << "\n"
              << "UndelineThickness = " << Double(inFontInfoDictionary.UnderlineThickness).ToString().c_str() << "\n";
}

void ShowPrivateInfoDictionary(const Type1PrivateDictionary &inPrivateDictionary)
{
    std::cout << "\nShowing Private Dictionary:\n"
              << "UniqueID = " << inPrivateDictionary.UniqueID << "\n"
              << "BlueValues = ";
    ShowIntVector(inPrivateDictionary.BlueValues);
    std::cout << "\nOtherBlues = ";
    ShowIntVector(inPrivateDictionary.OtherBlues);
    std::cout << "\nFamilyBlues = ";
    ShowIntVector(inPrivateDictionary.FamilyBlues);
    std::cout << "\nFamilyOtherBlues = ";
    ShowIntVector(inPrivateDictionary.FamilyOtherBlues);
    std::cout << "\nBlueScale = " << Double(inPrivateDictionary.BlueScale).ToString().c_str() << "\n"
              << "BlueShift = " << inPrivateDictionary.BlueShift << "\n"
              << "BlueFuzz = " << inPrivateDictionary.BlueFuzz << "\n"
              << "StdHW = " << Double(inPrivateDictionary.StdHW).ToString().c_str() << "\n"
              << "StdVW = " << Double(inPrivateDictionary.StdVW).ToString().c_str() << "\n"
              << "StemSnapH = ";
    ShowDoubleVector(inPrivateDictionary.StemSnapH);
    std::cout << "\nStemSnapW = ";
    ShowDoubleVector(inPrivateDictionary.StemSnapV);
    std::cout << "\nForceBold = " << PSBool(inPrivateDictionary.ForceBold).ToString().c_str() << "\n"
              << "LanguageGrup = " << inPrivateDictionary.LanguageGroup << "\n"
              << "lenIV = " << inPrivateDictionary.lenIV << "\n"
              << "RndStemUp = " << PSBool(inPrivateDictionary.RndStemUp).ToString().c_str() << "\n";
}

EStatusCode ShowDependencies(const std::string &inCharStringName, Type1Input *inType1Input)
{
    CharString1Dependencies dependencies;
    EStatusCode status = inType1Input->CalculateDependenciesForCharIndex(inCharStringName, dependencies);
    if (status != PDFHummus::eSuccess)
        return status;

    if (!dependencies.mCharCodes.empty() || !dependencies.mOtherSubrs.empty() || !dependencies.mSubrs.empty())
    {
        std::cout << "Glyph " << inCharStringName.c_str() << " has dependencies:\n";
        auto itBytes = dependencies.mCharCodes.begin();
        for (; itBytes != dependencies.mCharCodes.end(); ++itBytes)
            std::cout << "Seac Charcode " << *itBytes << "\n";
        auto itShort = dependencies.mOtherSubrs.begin();
        for (; itShort != dependencies.mOtherSubrs.end(); ++itShort)
            std::cout << "Other Subr " << *itShort << "\n";
        itShort = dependencies.mSubrs.begin();
        for (; itShort != dependencies.mSubrs.end(); ++itShort)
            std::cout << "Subr " << *itShort << "\n";
    }
    else
    {
        std::cout << "No dependencies for " << inCharStringName.c_str() << "\n";
    }
    return status;
}

EStatusCode SaveCharstringCode(const std::string &inCharStringName, Type1Input *inType1Input)
{
    OutputFile glyphFile;

    EStatusCode status = glyphFile.OpenFile(
        RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, std::string("glyphType1_") + inCharStringName + "_.txt"));
    if (status != PDFHummus::eSuccess)
        return status;

    CharStringType1Tracer tracer;

    status = tracer.TraceGlyphProgram(inCharStringName, inType1Input, glyphFile.GetOutputStream());

    glyphFile.CloseFile();

    return status;
}

TEST(Type1, Type1)
{
    EStatusCode status = PDFHummus::eSuccess;
    InputFile type1File;
    Type1Input type1Input;

    status = type1File.OpenFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/HLB_____.PFB"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = type1Input.ReadType1File(type1File.GetInputStream());
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // dump Font dictionary values
    ShowFontDictionary(type1Input.mFontDictionary);
    ShowFontInfoDictionary(type1Input.mFontInfoDictionary);
    ShowPrivateInfoDictionary(type1Input.mPrivateDictionary);

    // calculate dependencies for a,b,c,d and trace their code
    ASSERT_EQ(ShowDependencies("a", &type1Input), PDFHummus::eSuccess);
    ASSERT_EQ(ShowDependencies("b", &type1Input), PDFHummus::eSuccess);
    ASSERT_EQ(ShowDependencies("c", &type1Input), PDFHummus::eSuccess);
    ASSERT_EQ(ShowDependencies("d", &type1Input), PDFHummus::eSuccess);
    // show just abcd and notdef
    ASSERT_EQ(SaveCharstringCode(".notdef", &type1Input), PDFHummus::eSuccess);
    ASSERT_EQ(SaveCharstringCode("a", &type1Input), PDFHummus::eSuccess);
    ASSERT_EQ(SaveCharstringCode("b", &type1Input), PDFHummus::eSuccess);
    ASSERT_EQ(SaveCharstringCode("c", &type1Input), PDFHummus::eSuccess);
    ASSERT_EQ(SaveCharstringCode("d", &type1Input), PDFHummus::eSuccess);

    type1File.CloseFile();
}