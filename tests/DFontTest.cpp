/*
 Source File : DFontTest.cpp


 Copyright 2013 Gal Kahana PDFWriter

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
#include "PDFWriter.h"
#include "PageContentContext.h"
#include "TestHelper.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

TEST(FontPackages, DFont)
{
    PDFWriter pdfWriter;
    EStatusCode status;

    status =
        pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "DFontTest.pdf"), ePDFVersion13,
                           LogConfiguration(true, true, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "DFontTest.log")));
    ASSERT_EQ(status, eSuccess);

    PDFUsedFont *courierFonts[4];

    for (int i = 0; i < 4 && eSuccess == status; ++i)
    {
        courierFonts[i] =
            pdfWriter.GetFontForFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/courier.dfont"), i);
        ASSERT_NE(courierFonts[i], nullptr);
    }

    PDFPage page;
    page.SetMediaBox(PDFRectangle(0, 0, 595, 842));

    PageContentContext *contentContext = pdfWriter.StartPageContentContext(page);
    ASSERT_NE(contentContext, nullptr);

    for (int i = 0; i < 4; ++i)
    {
        contentContext->BT();
        contentContext->k(0, 0, 0, 1);
        contentContext->Tf(courierFonts[i], 1);
        contentContext->Tm(30, 0, 0, 30, 78.4252, 662.8997 - i * 100);
        EStatusCode encodingStatus = contentContext->Tj("Hello World!");
        ASSERT_EQ(encodingStatus, eSuccess);
        contentContext->ET();
    }

    status = pdfWriter.EndPageContentContext(contentContext);
    ASSERT_EQ(status, eSuccess);

    status = pdfWriter.WritePage(page);
    ASSERT_EQ(status, eSuccess);

    status = pdfWriter.EndPDF();
    ASSERT_EQ(status, eSuccess);
}
