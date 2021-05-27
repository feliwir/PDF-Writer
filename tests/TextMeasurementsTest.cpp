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

using namespace charta;

TEST(Text, TextMeasurements)
{
    EStatusCode status = eSuccess;
    PDFWriter pdfWriter;

    status = pdfWriter.StartPDF(
        RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "TextMeasurementsTest.pdf"), ePDFVersion13,
        LogConfiguration(true, true, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "TextMeasurementsTest.log")));
    ASSERT_EQ(status, charta::eSuccess);

    PDFPage page;
    page.SetMediaBox(charta::PagePresets::A4_Portrait);

    PageContentContext *cxt = pdfWriter.StartPageContentContext(page);
    PDFUsedFont *arialFont =
        pdfWriter.GetFontForFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/arial.ttf"));
    ASSERT_NE(arialFont, nullptr);

    AbstractContentContext::GraphicOptions pathStrokeOptions(
        AbstractContentContext::eStroke, AbstractContentContext::eRGB,
        AbstractContentContext::ColorValueForName("DarkMagenta"), 4);
    AbstractContentContext::TextOptions textOptions(arialFont, 14, AbstractContentContext::eGray, 0);

    PDFUsedFont::TextMeasures textDimensions = arialFont->CalculateTextDimensions("Hello World", 14);

    cxt->WriteText(10, 100, "Hello World", textOptions);
    DoubleAndDoublePairList pathPoints;
    pathPoints.push_back({10 + textDimensions.xMin, 98 + textDimensions.yMin});
    pathPoints.push_back({10 + textDimensions.xMax, 98 + textDimensions.yMin});
    cxt->DrawPath(pathPoints, pathStrokeOptions);
    pathPoints.clear();
    pathPoints.push_back({10 + textDimensions.xMin, 102 + textDimensions.yMax});
    pathPoints.push_back({10 + textDimensions.xMax, 102 + textDimensions.yMax});
    cxt->DrawPath(pathPoints, pathStrokeOptions);

    status = pdfWriter.EndPageContentContext(cxt);
    ASSERT_EQ(status, charta::eSuccess);

    status = pdfWriter.WritePage(page);
    ASSERT_EQ(status, charta::eSuccess);

    status = pdfWriter.EndPDF();
    ASSERT_EQ(status, charta::eSuccess);
}