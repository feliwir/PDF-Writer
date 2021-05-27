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
#include "TestHelper.h"

#include <gtest/gtest.h>

using namespace charta;

TEST(PDF, EmptyPagesPDF)
{
    PDFWriter pdfWriter;
    LogConfiguration logConfiguration(true, true, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "EmptyPagesLog.txt"));
    EStatusCode status;

    status = pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "EmptyPages.pdf"), ePDFVersion13,
                                logConfiguration);
    ASSERT_EQ(status, charta::eSuccess);

    PDFPage page;
    page.SetMediaBox(charta::PagePresets::A4_Portrait);

    for (int i = 0; i < 4 && charta::eSuccess == status; ++i)
    {
        status = pdfWriter.WritePage(page);
        ASSERT_EQ(status, charta::eSuccess);
    }

    status = pdfWriter.EndPDF();
    ASSERT_EQ(status, charta::eSuccess);
}
