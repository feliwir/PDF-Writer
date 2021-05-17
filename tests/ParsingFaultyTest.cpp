/*
Source File : FlateObjectDecodeTest.cpp


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
#include "EStatusCode.h"
#include "TestHelper.h"
#include "Trace.h"
#include "io/InputFile.h"
#include "objects/helpers/PDFPageInput.h"
#include "parsing/PDFParser.h"

#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <string>

using namespace PDFHummus;

static EStatusCode openPDF(const std::string &path)
{
    PDFParser parser;
    InputFile pdfFile;
    EStatusCode status = pdfFile.OpenFile(path);
    if (status != eSuccess)
        return status;

    return parser.StartPDFParsing(pdfFile.GetInputStream());
}

EStatusCode openPDFForNullPageTest(const std::string &path)
{
    PDFParser parser;
    InputFile pdfFile;
    EStatusCode status = pdfFile.OpenFile(path);
    if (status != eSuccess)
        return status;

    status = parser.StartPDFParsing(pdfFile.GetInputStream());
    if (status != eSuccess)
        return status;

    RefCountPtr<PDFDictionary> page = parser.ParsePage(33);
    if (page != nullptr)
        status = eFailure;

    return status;
}

EStatusCode openPDFForRotationTest(const std::string &path)
{
    PDFParser parser;
    InputFile pdfFile;
    EStatusCode status = pdfFile.OpenFile(path);
    if (status != eSuccess)
        return status;

    status = parser.StartPDFParsing(pdfFile.GetInputStream());
    if (status != eSuccess)
        return status;

    PDFPageInput pageInput(&parser, parser.ParsePage(0));
    if (pageInput.GetRotate() != -90)
        status = eFailure;

    return status;
}

TEST(Parsing, ParsingFaulty)
{
    Trace::DefaultTrace().SetLogSettings(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "parsingFaultyLog.txt"), true,
                                         true);

    ASSERT_EQ(openPDF(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/test3.pdf")), eSuccess);
    ASSERT_EQ(openPDF(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/test4.pdf")), eSuccess);
    ASSERT_EQ(openPDF(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/kids-as-reference.pdf")), eSuccess);
    ASSERT_EQ(openPDFForNullPageTest(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/unexpected.kids.array.pdf")),
              eSuccess);
    ASSERT_EQ(openPDFForRotationTest(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/wrong.rotation.pdf")),
              eSuccess);
    ASSERT_EQ(openPDF(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/1.unfamiliar.entry.type.pdf")), eSuccess);
    ASSERT_EQ(openPDF(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/2.unfamiliar.entry.type.pdf")), eSuccess);
    ASSERT_EQ(openPDF(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/nonZeroXref.pdf")), eSuccess);
}