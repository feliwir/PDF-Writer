/*
 Source File : MergeToPDFForm.cpp


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
#include "PDFFormXObject.h"
#include "PDFPage.h"
#include "PDFWriter.h"
#include "PageContentContext.h"
#include "TestHelper.h"
#include "XObjectContentContext.h"
#include "parsing/PDFDocumentCopyingContext.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace charta;

TEST(PDFEmbedding, MergeToPDFForm)
{
    PDFWriter pdfWriter;
    EStatusCode status;
    std::shared_ptr<PDFDocumentCopyingContext> copyingContext = nullptr;

    status = pdfWriter.StartPDF(
        RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "MergeToPDFForm.pdf"), ePDFVersion13,
        LogConfiguration(true, true, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "MergeToPDFForm.txt")));
    ASSERT_EQ(status, charta::eSuccess);

    // in this test we will merge 2 pages into a PDF form, and place it twice, forming a 2X2 design. amazing.

    PDFPage page;
    page.SetMediaBox(charta::PagePresets::A4_Portrait);

    copyingContext =
        pdfWriter.CreatePDFCopyingContext(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/Linearized.pdf"));
    ASSERT_EQ(status, charta::eSuccess);

    // create form for two pages.
    PDFFormXObject *newFormXObject = pdfWriter.StartFormXObject(PDFRectangle(0, 0, 297.5, 842));

    XObjectContentContext *xobjectContentContext = newFormXObject->GetContentContext();

    xobjectContentContext->q();
    xobjectContentContext->cm(0.5, 0, 0, 0.5, 0, 0);
    status = copyingContext->MergePDFPageToFormXObject(newFormXObject, 1);
    ASSERT_EQ(status, charta::eSuccess);
    xobjectContentContext->Q();

    xobjectContentContext->q();
    xobjectContentContext->cm(0.5, 0, 0, 0.5, 0, 421);
    status = copyingContext->MergePDFPageToFormXObject(newFormXObject, 0);
    ASSERT_EQ(status, charta::eSuccess);
    xobjectContentContext->Q();

    ObjectIDType formID = newFormXObject->GetObjectID();
    status = pdfWriter.EndFormXObjectAndRelease(newFormXObject);
    ASSERT_EQ(status, charta::eSuccess);

    // now place it in the page
    PageContentContext *pageContentContext = pdfWriter.StartPageContentContext(page);

    auto formName = page.GetResourcesDictionary().AddFormXObjectMapping(formID);

    pageContentContext->q();
    pageContentContext->Do(formName);
    pageContentContext->cm(1, 0, 0, 1, 297.5, 0);
    pageContentContext->Do(formName);
    pageContentContext->Q();

    status = pdfWriter.EndPageContentContext(pageContentContext);
    ASSERT_EQ(status, charta::eSuccess);

    status = pdfWriter.WritePage(page);
    ASSERT_EQ(status, charta::eSuccess);

    status = pdfWriter.EndPDF();
    ASSERT_EQ(status, charta::eSuccess);
}
