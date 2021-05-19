/*
   Source File : PDFEmbedTest.cpp


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
#include "PDFWriter.h"
#include "PageContentContext.h"
#include "TestHelper.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

TEST(PDFEmbedding, PDFEmbedTest)
{
    EStatusCode status;
    PDFWriter pdfWriter;

    status = pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "PDFEmbedTest.pdf"), ePDFVersion13);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // Create XObjects from PDF to embed
    EStatusCodeAndObjectIDTypeList result = pdfWriter.CreateFormXObjectsFromPDF(
        RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/XObjectContent.pdf"), PDFPageRange(), ePDFPageBoxMediaBox);
    ASSERT_EQ(result.first, PDFHummus::eSuccess);

    PDFPage page;
    page.SetMediaBox(PDFHummus::PagePresets::A4_Portrait);

    PageContentContext *contentContext = pdfWriter.StartPageContentContext(page);

    // place the first page in the top left corner of the document
    contentContext->q();
    contentContext->cm(0.5, 0, 0, 0.5, 0, 421);
    contentContext->Do(page.GetResourcesDictionary().AddFormXObjectMapping(result.second.front()));
    contentContext->Q();

    contentContext->G(0);
    contentContext->w(1);
    contentContext->re(0, 421, 297.5, 421);
    contentContext->S();

    // place the second page in the bottom right corner of the document
    contentContext->q();
    contentContext->cm(0.5, 0, 0, 0.5, 297.5, 0);
    contentContext->Do(page.GetResourcesDictionary().AddFormXObjectMapping(result.second.back()));
    contentContext->Q();

    contentContext->G(0);
    contentContext->w(1);
    contentContext->re(297.5, 0, 297.5, 421);
    contentContext->S();

    status = pdfWriter.EndPageContentContext(contentContext);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = pdfWriter.WritePage(page);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = pdfWriter.EndPDF();
    ASSERT_EQ(status, PDFHummus::eSuccess);
}