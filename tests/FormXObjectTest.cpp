/*
   Source File : FormXObjectTest.cpp


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
#include "XObjectContentContext.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

TEST(PDF, FormXObject)
{
    PDFWriter pdfWriter;
    EStatusCode status;

    status = pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "XObjectContent.pdf"), ePDFVersion13);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // define an xobject form to draw a 200X100 points red rectangle
    PDFFormXObject *xobjectForm = pdfWriter.StartFormXObject(PDFRectangle(0, 0, 200, 100));
    ObjectIDType formObjectID = xobjectForm->GetObjectID();

    // First page
    {
        PDFPage page;
        page.SetMediaBox(PDFHummus::PagePresets::A4_Portrait);

        PageContentContext *pageContentContext = pdfWriter.StartPageContentContext(page);
        ASSERT_NE(pageContentContext, nullptr);

        // draw a 100X100 points cyan square
        pageContentContext->q();
        pageContentContext->k(100, 0, 0, 0);
        pageContentContext->re(100, 500, 100, 100);
        pageContentContext->f();
        pageContentContext->Q();

        // pause stream to start writing a form xobject
        status = pdfWriter.PausePageContentContext(pageContentContext);
        ASSERT_EQ(status, PDFHummus::eSuccess);

        XObjectContentContext *xobjectContentContext = xobjectForm->GetContentContext();
        xobjectContentContext->q();
        xobjectContentContext->k(0, 100, 100, 0);
        xobjectContentContext->re(0, 0, 200, 100);
        xobjectContentContext->f();
        xobjectContentContext->Q();

        status = pdfWriter.EndFormXObjectAndRelease(xobjectForm);
        ASSERT_EQ(status, PDFHummus::eSuccess);

        std::string formXObjectName = page.GetResourcesDictionary().AddFormXObjectMapping(formObjectID);

        // continue page drawing, place the form in 200,600
        pageContentContext->q();
        pageContentContext->cm(1, 0, 0, 1, 200, 600);
        pageContentContext->Do(formXObjectName);
        pageContentContext->Q();

        // draw a gray line
        pageContentContext->q();
        pageContentContext->G(0.5);
        pageContentContext->w(3);
        pageContentContext->m(200, 600);
        pageContentContext->l(400, 400);
        pageContentContext->S();
        pageContentContext->Q();

        // place the form in another location
        pageContentContext->q();
        pageContentContext->cm(1, 0, 0, 1, 200, 200);
        pageContentContext->Do(formXObjectName);
        pageContentContext->Q();

        status = pdfWriter.EndPageContentContext(pageContentContext);
        ASSERT_EQ(status, PDFHummus::eSuccess);

        status = pdfWriter.WritePage(page);
        ASSERT_EQ(status, PDFHummus::eSuccess);
    }

    // 2nd page, just uses the form from the previous page
    {
        PDFPage page;
        page.SetMediaBox(PDFHummus::PagePresets::A4_Portrait);

        PageContentContext *pageContentContext = pdfWriter.StartPageContentContext(page);
        ASSERT_NE(pageContentContext, nullptr);

        auto formXObjectName = page.GetResourcesDictionary().AddFormXObjectMapping(formObjectID);

        // place the form in 300,500
        pageContentContext->q();
        pageContentContext->cm(1, 0, 0, 1, 300, 500);
        pageContentContext->Do(formXObjectName);
        pageContentContext->Q();

        status = pdfWriter.EndPageContentContext(pageContentContext);
        ASSERT_EQ(status, PDFHummus::eSuccess);

        status = pdfWriter.WritePage(page);
        ASSERT_EQ(status, PDFHummus::eSuccess);

        status = pdfWriter.EndPDF();
        ASSERT_EQ(status, PDFHummus::eSuccess);
    }
}