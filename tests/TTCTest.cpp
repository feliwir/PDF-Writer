/*
 Source File : TTCTest.cpp


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

TEST(FontPackages, TTC)
{
    PDFWriter pdfWriter;
    EStatusCode status;

    status = pdfWriter.StartPDF(
        RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "TTCTestLucidaGrande.pdf"), ePDFVersion13,
        LogConfiguration(true, true, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "TTCTestLucidaGrande.log")));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    PDFPage page;
    page.SetMediaBox(PDFHummus::PagePresets::A4_Portrait);

    PageContentContext *contentContext = pdfWriter.StartPageContentContext(page);
    ASSERT_NE(contentContext, nullptr);

    PDFUsedFont *fontLucidaGrande0 =
        pdfWriter.GetFontForFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/LucidaGrande.ttc"), 0);
    ASSERT_NE(fontLucidaGrande0, nullptr);

    PDFUsedFont *fontLucidaGrande1 =
        pdfWriter.GetFontForFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/LucidaGrande.ttc"), 1);
    ASSERT_NE(fontLucidaGrande1, nullptr);

    // Draw some text in Lucida Grande 0 (regular)
    contentContext->BT();
    contentContext->k(0, 0, 0, 1);
    contentContext->Tf(fontLucidaGrande0, 1);
    contentContext->Tm(30, 0, 0, 30, 78.4252, 662.8997);
    EStatusCode encodingStatus = contentContext->Tj("Hello World!");
    ASSERT_EQ(encodingStatus, PDFHummus::eSuccess);
    contentContext->ET();

    // Draw some text in Lucida Grande 0 (bold)
    contentContext->BT();
    contentContext->k(0, 0, 0, 1);
    contentContext->Tf(fontLucidaGrande1, 1);
    contentContext->Tm(30, 0, 0, 30, 78.4252, 462.8997);
    encodingStatus = contentContext->Tj("Hello World!");
    ASSERT_EQ(encodingStatus, PDFHummus::eSuccess);
    contentContext->ET();

    status = pdfWriter.EndPageContentContext(contentContext);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = pdfWriter.WritePage(page);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = pdfWriter.EndPDF();
    ASSERT_EQ(status, PDFHummus::eSuccess);
}