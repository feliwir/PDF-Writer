/*
 Source File : PageModifierTest.cpp


 Copyright 2013 Gal Kahana PDFWriter

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
#include "AbstractContentContext.h"
#include "PDFModifiedPage.h"
#include "PDFWriter.h"
#include "TestHelper.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

TEST(Modification, PageModifier)
{
    EStatusCode status = eSuccess;
    PDFWriter pdfWriter;

    // Modify existing page. first modify it's page box, then anoteher page content

    // open file for modification
    status = pdfWriter.ModifyPDF(
        RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/XObjectContent.pdf"), ePDFVersion13,
        RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "XObjectContentModified.pdf"),
        LogConfiguration(true, true, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "XObjectContentModified.log")));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    PDFModifiedPage modifiedPage(&pdfWriter, 0);

    AbstractContentContext *contentContext = modifiedPage.StartContentContext();
    AbstractContentContext::TextOptions opt(
        pdfWriter.GetFontForFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/arial.ttf")), 14,
        AbstractContentContext::eGray, 0);

    contentContext->WriteText(75, 805, "Test Text", opt);

    modifiedPage.EndContentContext();
    modifiedPage.WritePage();

    status = pdfWriter.EndPDF();
    ASSERT_EQ(status, PDFHummus::eSuccess);
}