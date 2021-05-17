/*
   Source File : LinksTest.cpp


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
#include "PDFFormXObject.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PDFWriter.h"
#include "PageContentContext.h"
#include "TestHelper.h"

#include <gtest/gtest.h>
#include <iostream>
using namespace PDFHummus;

TEST(PDF, Links)
{
    EStatusCode status;
    PDFWriter pdfWriter;

    status =
        pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "LinksTest.pdf"), ePDFVersion13,
                           LogConfiguration(true, true, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "LinksTest.log")));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    PDFPage page;
    page.SetMediaBox(PDFRectangle(0, 0, 595, 842));

    PDFFormXObject *soundCloudLogo = pdfWriter.CreateFormXObjectFromJPGFile(
        RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/soundcloud_logo.jpg"));

    PageContentContext *contentContext = pdfWriter.StartPageContentContext(page);
    ASSERT_NE(contentContext, nullptr);

    PDFUsedFont *font = pdfWriter.GetFontForFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/arial.ttf"));
    ASSERT_NE(font, nullptr);

    // Draw some text
    contentContext->BT();
    contentContext->k(0, 0, 0, 1);
    contentContext->Tf(font, 1);
    contentContext->Tm(11, 0, 0, 11, 90.024, 709.54);
    EStatusCode encodingStatus = contentContext->Tj("http://pdfhummus.com");
    if (encodingStatus != PDFHummus::eSuccess)
        std::cout << "Could not find some of the glyphs for this font (arial)";
    // continue even if failed...want to see how it looks like
    contentContext->ET();

    // Draw soundcloud loog
    contentContext->q();
    contentContext->cm(0.5, 0, 0, 0.5, 90.024, 200);
    contentContext->Do(page.GetResourcesDictionary().AddFormXObjectMapping(soundCloudLogo->GetObjectID()));
    contentContext->Q();

    status = pdfWriter.EndPageContentContext(contentContext);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    delete soundCloudLogo;

    // now let's attach some links.

    // first, the link for the test:
    pdfWriter.AttachURLLinktoCurrentPage("http://www.pdfhummus.com", PDFRectangle(87.75, 694.56, 198.76, 720));

    // second, link for the logo.
    pdfWriter.AttachURLLinktoCurrentPage("http://www.soundcloud.com", PDFRectangle(90.024, 200, 367.524, 375));

    status = pdfWriter.WritePage(page);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = pdfWriter.EndPDF();
    ASSERT_EQ(status, PDFHummus::eSuccess);
}