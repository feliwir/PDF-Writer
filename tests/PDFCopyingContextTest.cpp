/*
   Source File : PDFCopyingContextTest.cpp


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
#include "parsing/PDFDocumentCopyingContext.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

TEST(PDFEmbedding, PDFCopyingContext)
{
    EStatusCode status;
    PDFWriter pdfWriter;
    std::shared_ptr<PDFDocumentCopyingContext> copyingContext = nullptr;

    status = pdfWriter.StartPDF(
        RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "PDFCopyingContextTest.pdf"), ePDFVersion13,
        LogConfiguration(true, true, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "PDFCopyingContextTest.txt")));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    copyingContext =
        pdfWriter.CreatePDFCopyingContext(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/BasicTIFFImagesTest.pdf"));
    ASSERT_NE(copyingContext, nullptr);

    EStatusCodeAndObjectIDType result = copyingContext->AppendPDFPageFromPDF(1);
    ASSERT_EQ(result.first, PDFHummus::eSuccess);

    result = copyingContext->AppendPDFPageFromPDF(18);
    ASSERT_EQ(result.first, PDFHummus::eSuccess);

    result = copyingContext->AppendPDFPageFromPDF(4);
    ASSERT_EQ(result.first, PDFHummus::eSuccess);

    copyingContext->End(); // delete will call End() as well...so can avoid
    copyingContext = nullptr;

    status = pdfWriter.EndPDF();
    ASSERT_EQ(status, PDFHummus::eSuccess);
}