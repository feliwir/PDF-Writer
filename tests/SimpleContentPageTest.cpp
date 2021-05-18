/*
   Source File : SimpleContentPageTest.cpp


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

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

/*
    Creates a single page PDF with some drawings
*/
TEST(PDF, SimpleContentPage)
{
    PDFWriter pdfWriter;
    EStatusCode status;
    status = pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "SimpleContent.pdf"), ePDFVersion13);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    PDFPage page;
    page.SetMediaBox(PDFRectangle(0, 0, 595, 842));

    PageContentContext *contentContext = pdfWriter.StartPageContentContext(page);
    ASSERT_NE(contentContext, nullptr);

    // draw a 100X100 points cyan square
    contentContext->q();
    contentContext->k(100, 0, 0, 0);
    contentContext->re(100, 500, 100, 100);
    contentContext->f();
    contentContext->Q();

    // force stream change
    status = pdfWriter.PausePageContentContext(contentContext);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // draw a 200X100 points red rectangle
    contentContext->q();
    contentContext->k(0, 100, 100, 0);
    contentContext->re(200, 600, 200, 100);
    contentContext->f();
    contentContext->Q();

    // draw a gray line
    contentContext->q();
    contentContext->G(0.5);
    contentContext->w(3);
    contentContext->m(200, 600);
    contentContext->l(400, 400);
    contentContext->S();
    contentContext->Q();

    status = pdfWriter.EndPageContentContext(contentContext);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = pdfWriter.WritePage(page);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = pdfWriter.EndPDF();
    ASSERT_EQ(status, PDFHummus::eSuccess);
}