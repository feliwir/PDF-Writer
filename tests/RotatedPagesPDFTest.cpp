/*
   Source File : RotatedPagesPDF.cpp


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
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PDFWriter.h"
#include "PageContentContext.h"
#include "TestHelper.h"
#include "objects/helpers/PDFPageInput.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

TEST(PDF, RotatedPagesPDF)
{
    LogConfiguration logConfiguration(true, true, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "RotatedPagesLog.txt"));
    EStatusCode status;

    // PDF Page rotation writing
    PDFWriter pdfWriter;
    status = pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "RotatedPages.pdf"), ePDFVersion13,
                                logConfiguration);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    AbstractContentContext::TextOptions textOptions(
        pdfWriter.GetFontForFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/arial.ttf")), 14,
        AbstractContentContext::eGray, 0);

    for (int i = 0; i < 6; ++i)
    {
        PDFPage page;
        page.SetMediaBox(PDFHummus::PagePresets::A4_Portrait);

        // This is invalid rotation and should not be applied
        page.SetRotate(33);
        ASSERT_EQ(page.GetRotate().second, 0);

        page.SetRotate(i * 90);

        std::ostringstream s;
        s << "Page rotated by " << i * 90 << " degrees.";

        PageContentContext *cxt = pdfWriter.StartPageContentContext(page);
        cxt->WriteText(75, 805, s.str(), textOptions);
        status = pdfWriter.EndPageContentContext(cxt);
        ASSERT_EQ(status, PDFHummus::eSuccess);

        status = pdfWriter.WritePage(page);
        ASSERT_EQ(status, PDFHummus::eSuccess);
    }

    status = pdfWriter.EndPDF();
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // PDF page rotation copy
    status = pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "RotatedPagesCopy.pdf"), ePDFVersion13);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    EStatusCodeAndObjectIDTypeList result;

    // append pages
    result = pdfWriter.AppendPDFPagesFromPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "RotatedPages.pdf"),
                                             PDFPageRange());
    ASSERT_EQ(result.first, PDFHummus::eSuccess);

    status = pdfWriter.EndPDF();
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // PDF Page rotation parsing

    InputFile pdfFile;
    PDFParser pdfParser;

    status = pdfFile.OpenFile(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "RotatedPagesCopy.pdf"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = pdfParser.StartPDFParsing(pdfFile.GetInputStream());
    ASSERT_EQ(status, PDFHummus::eSuccess);

    ASSERT_EQ(pdfParser.GetPagesCount(), 6);

    for (unsigned long i = 0; i < pdfParser.GetPagesCount(); ++i)
    {
        auto page = pdfParser.ParsePage(i);
        ASSERT_NE(page.GetPtr(), nullptr);

        PDFPageInput input(&pdfParser, page);
        ASSERT_EQ(input.GetRotate(), i * 90);
    }
}