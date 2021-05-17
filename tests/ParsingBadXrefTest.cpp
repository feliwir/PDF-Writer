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
#include "io/InputFile.h"
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

TEST(Parsing, ParsingBadXref)
{
    EStatusCode status = openPDF(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/test_bad_xref.pdf"));
    // This should be a failure
    ASSERT_EQ(status, PDFHummus::eFailure);
}
