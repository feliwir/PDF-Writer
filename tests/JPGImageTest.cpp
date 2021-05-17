/*
   Source File : JPGImageTest.cpp


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

#ifndef PDFHUMMUS_NO_DCT

#include "PDFFormXObject.h"
#include "PDFImageXObject.h"
#include "PDFPage.h"
#include "PDFWriter.h"
#include "PageContentContext.h"
#include "TestHelper.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

TEST(PDFImage, JPGImage)
{
    PDFWriter pdfWriter;
    EStatusCode status;

    status = pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "BasicJPGImagesTest.pdf"), ePDFVersion13);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    PDFPage page;
    page.SetMediaBox(PDFRectangle(0, 0, 595, 842));

    PageContentContext *pageContentContext = pdfWriter.StartPageContentContext(page);
    ASSERT_NE(pageContentContext, nullptr);

    // draw a rectangle
    pageContentContext->q();
    pageContentContext->k(100, 0, 0, 0);
    pageContentContext->re(500, 0, 100, 100);
    pageContentContext->f();
    pageContentContext->Q();

    // pause stream to start writing the image
    status = pdfWriter.PausePageContentContext(pageContentContext);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // Create image xobject from
    PDFImageXObject *imageXObject = pdfWriter.CreateImageXObjectFromJPGFile(
        RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/otherStage.JPG"));
    ASSERT_NE(imageXObject, nullptr);

    // continue page drawing size the image to 500,400
    pageContentContext->q();
    pageContentContext->cm(500, 0, 0, 400, 0, 0);
    pageContentContext->Do(page.GetResourcesDictionary().AddImageXObjectMapping(imageXObject));
    pageContentContext->Q();

    delete imageXObject;

    // now do the same with a form xobject
    // pause stream to start writing the image
    status = pdfWriter.PausePageContentContext(pageContentContext);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    PDFFormXObject *formXObject = pdfWriter.CreateFormXObjectFromJPGFile(
        RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/otherStage.JPG"));
    ASSERT_NE(formXObject, nullptr);

    // continue page drawing size the image to 500,400
    pageContentContext->q();
    pageContentContext->cm(1, 0, 0, 1, 0, 400);
    pageContentContext->Do(page.GetResourcesDictionary().AddFormXObjectMapping(formXObject->GetObjectID()));
    pageContentContext->Q();

    delete formXObject;

    status = pdfWriter.EndPageContentContext(pageContentContext);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = pdfWriter.WritePage(page);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = pdfWriter.EndPDF();
    ASSERT_EQ(status, PDFHummus::eSuccess);
}

#endif