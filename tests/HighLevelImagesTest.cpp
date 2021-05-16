/*
   Source File : TestMeasurementsTest.cpp


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

#ifndef PDFHUMMUS_NO_TIFF

#include "PDFPage.h"
#include "PDFWriter.h"
#include "PageContentContext.h"
#include "TestHelper.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

TEST(PDFImages, HighLevelImages)
{
    EStatusCode status = eSuccess;
    PDFWriter pdfWriter;

    status = pdfWriter.StartPDF(
        RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "HighLevelImages.pdf"), ePDFVersion13,
        LogConfiguration(true, true, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "HighLevelImages.log")));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    auto *page = new PDFPage();
    page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

    PageContentContext *cxt = pdfWriter.StartPageContentContext(page);

    // simple image placement
    cxt->DrawImage(10, 10, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/soundcloud_logo.jpg"));
    cxt->DrawImage(10, 500, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/cramps.tif"));
    cxt->DrawImage(0, 0, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/XObjectContent.pdf"));

    status = pdfWriter.EndPageContentContext(cxt);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = pdfWriter.WritePageAndRelease(page);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    page = new PDFPage();
    page->SetMediaBox(PDFRectangle(0, 0, 595, 842));
    cxt = pdfWriter.StartPageContentContext(page);

    AbstractContentContext::ImageOptions opt1;
    opt1.imageIndex = 2;
    cxt->DrawImage(10, 10, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/multipage.tif"), opt1);
    AbstractContentContext::ImageOptions opt2;
    opt2.transformationMethod = AbstractContentContext::eMatrix;
    opt2.matrix[0] = opt2.matrix[3] = 0.25;
    cxt->DrawImage(10, 10, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/soundcloud_logo.jpg"), opt2);
    AbstractContentContext::ImageOptions opt3;
    opt3.transformationMethod = AbstractContentContext::eFit;
    opt3.boundingBoxHeight = 100;
    opt3.boundingBoxWidth = 100;
    cxt->DrawImage(0, 0, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/XObjectContent.pdf"), opt3);
    opt3.fitProportional = true;
    cxt->DrawImage(100, 100, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/XObjectContent.pdf"), opt3);

    // draw frames for soundcloud_logo.jpg and the 2 fitted version of XObjectContent.pdf
    AbstractContentContext::GraphicOptions pathStrokeOptions(
        AbstractContentContext::eStroke, AbstractContentContext::eRGB,
        AbstractContentContext::ColorValueForName("DarkMagenta"), 4);
    cxt->DrawRectangle(0, 0, 100, 100, pathStrokeOptions);
    cxt->DrawRectangle(100, 100, 100, 100, pathStrokeOptions);

    DoubleAndDoublePair jpgDimensions =
        pdfWriter.GetImageDimensions(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/soundcloud_logo.jpg"));
    cxt->DrawRectangle(10, 10, jpgDimensions.first / 4, jpgDimensions.second / 4, pathStrokeOptions);

    status = pdfWriter.EndPageContentContext(cxt);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = pdfWriter.WritePageAndRelease(page);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = pdfWriter.EndPDF();
    ASSERT_EQ(status, PDFHummus::eSuccess);
}

#endif