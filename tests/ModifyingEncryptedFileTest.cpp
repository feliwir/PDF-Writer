/*
 Source File : ModifyingEncryptedFile.cpp


 Copyright 2012 Gal Kahana PDFWriter

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
#include "PDFPage.h"
#include "PDFWriter.h"
#include "PageContentContext.h"
#include "TestHelper.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

EStatusCode RunTest(PDFWriter &pdfWriter)
{

    EStatusCode status = eSuccess;

    /*
    modify a password protected file. retain the same protection, just add some content
    */
    // modify first page to include text
    {
        PDFModifiedPage modifiedPage(&pdfWriter, 0);

        AbstractContentContext *contentContext = modifiedPage.StartContentContext();
        AbstractContentContext::TextOptions opt(
            pdfWriter.GetFontForFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/arial.ttf")), 14,
            AbstractContentContext::eGray, 0);

        contentContext->WriteText(10, 805, "new text on encrypted page!", opt);

        modifiedPage.EndContentContext();
        modifiedPage.WritePage();
    }

    // add new page with an image
    {
        PDFPage page;
        page.SetMediaBox(PDFHummus::PagePresets::A4_Portrait);

        PageContentContext *contentContext = pdfWriter.StartPageContentContext(page);
        if (contentContext == nullptr)
            return PDFHummus::eFailure;

        contentContext->DrawImage(10, 300,
                                  RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/soundcloud_logo.jpg"));

        status = pdfWriter.EndPageContentContext(contentContext);
        if (status != PDFHummus::eSuccess)
            return status;

        status = pdfWriter.WritePage(page);
        if (status != PDFHummus::eSuccess)
            return status;
    }

    return pdfWriter.EndPDF();
}

TEST(Modification, ModifyingEncryptedFile)
{
    EStatusCode status = eSuccess;

    {
        PDFWriter pdfWriter;
        status = pdfWriter.ModifyPDF(
            RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/PDFWithPassword.pdf"), ePDFVersion13,
            RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "PDFWithPasswordEcnryptedModified.pdf"),
            LogConfiguration(true, true,
                             RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "PDFWithPasswordEcnryptedModified.log")),
            PDFCreationSettings(true, true, EncryptionOptions("user", 0, "")));
        ASSERT_EQ(status, eSuccess);

        status = RunTest(pdfWriter);
        ASSERT_EQ(status, eSuccess);
    }

    {
        PDFWriter pdfWriter;
        status = pdfWriter.ModifyPDF(
            RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/china.pdf"), ePDFVersion13,
            RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "chinaEcnryptedModified.pdf"),
            LogConfiguration(true, true, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "chinaEcnryptedModified.log")),
            PDFCreationSettings(true, true));
        ASSERT_EQ(status, eSuccess);

        status = RunTest(pdfWriter);
        ASSERT_EQ(status, eSuccess);
    }
}