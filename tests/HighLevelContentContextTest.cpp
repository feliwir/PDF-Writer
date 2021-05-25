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
#include "PDFPage.h"
#include "PDFUsedFont.h"
#include "PDFWriter.h"
#include "PageContentContext.h"
#include "TestHelper.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

TEST(PDF, HighLevelContentContext)
{
    EStatusCode status = eSuccess;
    PDFWriter pdfWriter;

    status = pdfWriter.StartPDF(
        RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "HighLevelContentContext.pdf"), ePDFVersion13,
        LogConfiguration(true, true, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "HighLevelContentContext.log")));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    PDFPage page;
    page.SetMediaBox(PDFHummus::PagePresets::A4_Portrait);

    PageContentContext *cxt = pdfWriter.StartPageContentContext(page);

    AbstractContentContext::TextOptions textOptions(
        pdfWriter.GetFontForFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/arial.ttf")), 14,
        AbstractContentContext::eGray, 0);
    AbstractContentContext::GraphicOptions pathFillOptions(AbstractContentContext::eFill, AbstractContentContext::eCMYK,
                                                           0xFF000000);
    AbstractContentContext::GraphicOptions pathStrokeOptions(
        AbstractContentContext::eStroke, AbstractContentContext::eRGB,
        AbstractContentContext::ColorValueForName("DarkMagenta"), 4);

    DoubleAndDoublePairList pathPoints;

    // draw path
    pathPoints.push_back({75, 640});
    pathPoints.push_back({149, 800});
    pathPoints.push_back({225, 640});
    cxt->DrawPath(pathPoints, pathFillOptions);
    pathPoints.clear();
    pathPoints.push_back({75, 540});
    pathPoints.push_back({110, 440});
    pathPoints.push_back({149, 540});
    pathPoints.push_back({188, 440});
    pathPoints.push_back({223, 540});
    cxt->DrawPath(pathPoints, pathStrokeOptions);

    // draw square
    cxt->DrawSquare(375, 640, 120, pathFillOptions);
    cxt->DrawSquare(375, 440, 120, pathStrokeOptions);

    // draw rectangle
    cxt->DrawRectangle(375, 220, 50, 160, pathFillOptions);
    cxt->DrawRectangle(375, 10, 50, 160, pathStrokeOptions);

    // draw circle
    cxt->DrawCircle(149, 300, 80, pathFillOptions);
    cxt->DrawCircle(149, 90, 80, pathStrokeOptions);

    // wrote text (writing labels for each of the shapes)
    cxt->WriteText(75, 805, "Paths", textOptions);
    cxt->WriteText(375, 805, "Squares", textOptions);
    cxt->WriteText(375, 400, "Rectangles", textOptions);
    cxt->WriteText(75, 400, "Circles", textOptions);

    status = pdfWriter.EndPageContentContext(cxt);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = pdfWriter.WritePage(page);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = pdfWriter.EndPDF();
    ASSERT_EQ(status, PDFHummus::eSuccess);
}