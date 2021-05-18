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

#ifndef PDFHUMMUS_NO_PNG

#include "PDFFormXObject.h"
#include "PDFPage.h"
#include "PDFWriter.h"
#include "PageContentContext.h"
#include "TestHelper.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

EStatusCode RunImageTest(const std::string &inImageName)
{
    PDFWriter pdfWriter;
    EStatusCode status;

    status = pdfWriter.StartPDF(
        RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "PNGTest_" + inImageName + ".pdf"), ePDFVersion14,
        LogConfiguration(true, true, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "PNGTest_" + inImageName + ".log")));
    if (status != PDFHummus::eSuccess)
        return status;

    PDFPage page;
    page.SetMediaBox(PDFRectangle(0, 0, 595, 842));

    PageContentContext *pageContentContext = pdfWriter.StartPageContentContext(page);
    if (pageContentContext == nullptr)
        return eFailure;

    // place a large red rectangle all over the page
    AbstractContentContext::GraphicOptions pathFillOptions(AbstractContentContext::eFill, AbstractContentContext::eRGB,
                                                           0xFF0000);
    pageContentContext->DrawRectangle(0, 0, 595, 842, pathFillOptions);

    // place the image on top...hopefully we can see soem transparency
    AbstractContentContext::ImageOptions imageOptions;
    imageOptions.transformationMethod = AbstractContentContext::eMatrix;
    imageOptions.matrix[0] = imageOptions.matrix[3] = 0.5;
    pageContentContext->DrawImage(
        10, 200, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/png/" + inImageName + ".png"), imageOptions);

    status = pdfWriter.EndPageContentContext(pageContentContext);
    if (status != PDFHummus::eSuccess)
        return status;

    status = pdfWriter.WritePage(page);
    if (status != PDFHummus::eSuccess)
        return status;

    return pdfWriter.EndPDF();
}

TEST(PDFImages, PNGImage)
{
    EStatusCode status;

    ASSERT_EQ(RunImageTest("original"), eSuccess);
    ASSERT_EQ(RunImageTest("original_transparent"), eSuccess);
    ASSERT_EQ(RunImageTest("gray-alpha-8-linear"), eSuccess);
    ASSERT_EQ(RunImageTest("gray-16-linear"), eSuccess);
    ASSERT_EQ(RunImageTest("pnglogo-grr"), eSuccess);
}

#endif
