/*
   Source File : ShutDownRestartTest.cpp


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

using namespace charta;

TEST(PDF, ShutDownRestart)
{
    EStatusCode status;

    {
        PDFWriter pdfWriterA;
        status = pdfWriterA.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "SimpleContentShutdownRestart.pdf"),
                                     ePDFVersion13);
        ASSERT_EQ(status, charta::eSuccess);

        PDFPage page;
        page.SetMediaBox(charta::PagePresets::A4_Portrait);

        PDFUsedFont *font =
            pdfWriterA.GetFontForFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/arial.ttf"));
        ASSERT_NE(font, nullptr);

        PageContentContext *contentContext = pdfWriterA.StartPageContentContext(page);
        ASSERT_NE(contentContext, nullptr);

        // draw a 100X100 points cyan square
        contentContext->q();
        contentContext->k(100, 0, 0, 0);
        contentContext->re(100, 500, 100, 100);
        contentContext->f();
        contentContext->Q();

        // force stream change
        status = pdfWriterA.PausePageContentContext(contentContext);
        ASSERT_EQ(status, charta::eSuccess);

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

        // Draw some text
        contentContext->BT();
        contentContext->k(0, 0, 0, 1);
        contentContext->Tf(font, 1);
        contentContext->Tm(30, 0, 0, 30, 78.4252, 662.8997);

        EStatusCode encodingStatus = contentContext->Tj("hello world");
        ASSERT_EQ(encodingStatus, charta::eSuccess);

        // continue even if failed...want to see how it looks like
        contentContext->ET();

        status = pdfWriterA.EndPageContentContext(contentContext);
        ASSERT_EQ(status, charta::eSuccess);

        status = pdfWriterA.WritePage(page);
        ASSERT_EQ(status, charta::eSuccess);

        status = pdfWriterA.Shutdown(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "ShutDownRestartState.txt"));
        ASSERT_EQ(status, charta::eSuccess);
    }
    {
        PDFWriter pdfWriterB;
        status =
            pdfWriterB.ContinuePDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "SimpleContentShutdownRestart.pdf"),
                                   RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "ShutDownRestartState.txt"));
        ASSERT_EQ(status, charta::eSuccess);

        PDFPage page;
        page.SetMediaBox(charta::PagePresets::A4_Portrait);

        PageContentContext *pageContentContext = pdfWriterB.StartPageContentContext(page);
        ASSERT_NE(pageContentContext, nullptr);

        // draw a 100X100 points cyan square
        pageContentContext->q();
        pageContentContext->k(100, 0, 0, 0);
        pageContentContext->re(100, 500, 100, 100);
        pageContentContext->f();
        pageContentContext->Q();

        // pause stream to start writing a form xobject
        status = pdfWriterB.PausePageContentContext(pageContentContext);
        ASSERT_EQ(status, charta::eSuccess);

        // define an xobject form to draw a 200X100 points red rectangle
        PDFFormXObject *xobjectForm = pdfWriterB.StartFormXObject(PDFRectangle(0, 0, 200, 100));
        ObjectIDType formObjectID = xobjectForm->GetObjectID();

        XObjectContentContext *xobjectContentContext = xobjectForm->GetContentContext();
        xobjectContentContext->q();
        xobjectContentContext->k(0, 100, 100, 0);
        xobjectContentContext->re(0, 0, 200, 100);
        xobjectContentContext->f();
        xobjectContentContext->Q();

        status = pdfWriterB.EndFormXObjectAndRelease(xobjectForm);
        ASSERT_EQ(status, charta::eSuccess);

        auto formXObjectName = page.GetResourcesDictionary().AddFormXObjectMapping(formObjectID);

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

        status = pdfWriterB.EndPageContentContext(pageContentContext);
        ASSERT_EQ(status, charta::eSuccess);

        status = pdfWriterB.WritePage(page);
        ASSERT_EQ(status, charta::eSuccess);

        status = pdfWriterB.EndPDF();
        ASSERT_EQ(status, charta::eSuccess);
    }
}