/*
   Source File : AppendSpecialPagesTest.cpp


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
#include "PDFWriter.h"
#include "TestHelper.h"
#include <gtest/gtest.h>

#include <iostream>

using namespace PDFHummus;

TEST(PDFEmbedding, ApppendSpecialPages)
{
    EStatusCode status;
    PDFWriter pdfWriter;

    status = pdfWriter.StartPDF(
        RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "AppendSpecialPagesTest.pdf"), ePDFVersion13,
        LogConfiguration(true, true, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "AppendSpecialPagesTestLog.txt")));

    ASSERT_EQ(status, PDFHummus::eSuccess);

    EStatusCodeAndObjectIDTypeList result;

    result = pdfWriter.AppendPDFPagesFromPDF(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/Protected.pdf"),
                                             PDFPageRange());
    ASSERT_NE(result.first, PDFHummus::eSuccess);

    result = pdfWriter.AppendPDFPagesFromPDF(
        RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/ObjectStreamsModified.pdf"), PDFPageRange());
    ASSERT_EQ(result.first, PDFHummus::eSuccess);

    result = pdfWriter.AppendPDFPagesFromPDF(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/ObjectStreams.pdf"),
                                             PDFPageRange());
    ASSERT_EQ(result.first, PDFHummus::eSuccess);

    result = pdfWriter.AppendPDFPagesFromPDF(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/AddedItem.pdf"),
                                             PDFPageRange());
    ASSERT_EQ(result.first, PDFHummus::eSuccess);

    result = pdfWriter.AppendPDFPagesFromPDF(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/AddedPage.pdf"),
                                             PDFPageRange());
    ASSERT_EQ(result.first, PDFHummus::eSuccess);

    result = pdfWriter.AppendPDFPagesFromPDF(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/MultipleChange.pdf"),
                                             PDFPageRange());
    ASSERT_EQ(result.first, PDFHummus::eSuccess);

    result = pdfWriter.AppendPDFPagesFromPDF(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/RemovedItem.pdf"),
                                             PDFPageRange());
    ASSERT_EQ(result.first, PDFHummus::eSuccess);

    result = pdfWriter.AppendPDFPagesFromPDF(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/Linearized.pdf"),
                                             PDFPageRange());
    ASSERT_EQ(result.first, PDFHummus::eSuccess);

    status = pdfWriter.EndPDF();
    ASSERT_EQ(status, PDFHummus::eSuccess);
}