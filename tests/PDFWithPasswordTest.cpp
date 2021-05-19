/*
   Source File : EmptyPagesPDF.cpp


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

EStatusCode RunTest(bool inUseAES)
{
    PDFWriter pdfWriter;
    LogConfiguration logConfiguration(true, true,
                                      RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, inUseAES
                                                                                       ? "PDFWithPasswordAESLog.txt"
                                                                                       : "PDFWithPasswordRC4Log.txt"));
    EStatusCode status;

    status = pdfWriter.StartPDF(
        RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, inUseAES ? "PDFWithPasswordAES.pdf" : "PDFWithPasswordRC4.pdf"),
        inUseAES ? ePDFVersion16 : ePDFVersion14, logConfiguration,
        PDFCreationSettings(true, true,
                            EncryptionOptions("user", 4, "owner"))); // 4 should translate to -3900 in actual PDF
    if (status != PDFHummus::eSuccess)
        return status;

    // create simple page
    PDFPage page;
    page.SetMediaBox(PDFHummus::PagePresets::A4_Portrait);

    PageContentContext *cxt = pdfWriter.StartPageContentContext(page);

    // draw an image
    cxt->DrawImage(10, 100, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/soundcloud_logo.jpg"));

    // place some text
    AbstractContentContext::TextOptions textOptions(
        pdfWriter.GetFontForFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/arial.ttf")), 14,
        AbstractContentContext::eGray, 0);
    cxt->WriteText(10, 50, "Hello", textOptions);

    status = pdfWriter.EndPageContentContext(cxt);
    if (status != PDFHummus::eSuccess)
        return status;

    status = pdfWriter.WritePage(page);

    if (status != PDFHummus::eSuccess)
        return status;

    return pdfWriter.EndPDF();
}

TEST(Xcryption, PDFWithPassword)
{
    EStatusCode status;

    status = RunTest(false);
    ASSERT_EQ(status, eSuccess);

    status = RunTest(true);
    ASSERT_EQ(status, eSuccess);
}