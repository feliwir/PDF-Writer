/*
 Source File : CopyingAndMergingEmptyPagesTest.cpp


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

using namespace PDFHummus;

EStatusCode PrepareSampleEmptyPDF(const std::string &inEmptyFileName)
{
    PDFWriter pdfWriter;
    EStatusCode status;

    status = pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, inEmptyFileName), ePDFVersion13);
    if (status != eSuccess)
        return status;

    PDFPage *page = new PDFPage();
    page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

    status = pdfWriter.WritePageAndRelease(page);
    if (status != eSuccess)
        return status;
    status = pdfWriter.EndPDF();
    if (status != eSuccess)
        return status;

    return status;
}

PDFHummus::EStatusCode CreateFormFromEmptyPage(const std::string &inEmptyFileName)
{
    EStatusCode status;
    PDFWriter pdfWriter;

    status =
        pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "CreateFormFromEmptyPage.pdf"), ePDFVersion13);
    if (status != eSuccess)
        return status;

    // Create XObjects from PDF to embed
    EStatusCodeAndObjectIDTypeList result = pdfWriter.CreateFormXObjectsFromPDF(
        RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, inEmptyFileName), PDFPageRange(), ePDFPageBoxMediaBox);
    if (result.first != PDFHummus::eSuccess)
        return status;

    PDFPage *page = new PDFPage();
    page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

    PageContentContext *contentContext = pdfWriter.StartPageContentContext(page);

    // place the [empty] page in the top left corner of the document
    contentContext->q();
    contentContext->cm(0.5, 0, 0, 0.5, 0, 421);
    contentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(result.second.front()));
    contentContext->Q();

    contentContext->G(0);
    contentContext->w(1);
    contentContext->re(0, 421, 297.5, 421);
    contentContext->S();

    status = pdfWriter.EndPageContentContext(contentContext);
    if (status != eSuccess)
        return status;

    status = pdfWriter.WritePageAndRelease(page);
    if (status != eSuccess)
        return status;

    status = pdfWriter.EndPDF();
    if (status != eSuccess)
        return status;

    return status;
}

PDFHummus::EStatusCode CreatePageFromEmptyPage(const std::string &inEmptyFileName)
{
    EStatusCode status;

    PDFWriter pdfWriter;

    status =
        pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "CreatePageFromEmptyPage.pdf"), ePDFVersion13);
    if (status != eSuccess)
        return status;

    EStatusCodeAndObjectIDTypeList result;

    // append empty page
    result =
        pdfWriter.AppendPDFPagesFromPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, inEmptyFileName), PDFPageRange());
    if (status != eSuccess)
        return status;

    // append other pages
    result = pdfWriter.AppendPDFPagesFromPDF(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/XObjectContent.pdf"),
                                             PDFPageRange());
    if (status != eSuccess)
        return status;

    status = pdfWriter.EndPDF();
    if (status != eSuccess)
        return status;

    return status;
}

PDFHummus::EStatusCode MergeEmptyPageToPage(const std::string &inEmptyFileName)
{
    PDFWriter pdfWriter;
    EStatusCode status;

    status =
        pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "MergeEmptyPageToPage.pdf"), ePDFVersion13);
    if (status != eSuccess)
        return status;

    PDFPage *page = new PDFPage();
    page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

    PDFUsedFont *font = pdfWriter.GetFontForFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/arial.ttf"));
    if (!font)
    {
        return PDFHummus::eFailure;
    }

    PageContentContext *pageContent = pdfWriter.StartPageContentContext(page);

    pageContent->BT();
    pageContent->k(0, 0, 0, 1);
    pageContent->Tf(font, 30);
    pageContent->Tm(1, 0, 0, 1, 10, 600);
    status = pageContent->Tj("Testing file merge");
    if (status != eSuccess)
        return status;
    pageContent->ET();

    pageContent->q();
    pageContent->cm(0.5, 0, 0, 0.5, 0, 0);

    PDFPageRange singlePageRange;
    singlePageRange.mType = PDFPageRange::eRangeTypeSpecific;
    singlePageRange.mSpecificRanges.push_back(ULongAndULong(0, 0));

    status = pdfWriter.MergePDFPagesToPage(page, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, inEmptyFileName),
                                           singlePageRange);
    if (status != eSuccess)
        return status;

    pageContent->Q();

    pageContent->q();
    pageContent->cm(1, 0, 0, 1, 30, 500);
    pageContent->k(0, 100, 100, 0);
    pageContent->re(0, 0, 200, 100);
    pageContent->f();
    pageContent->Q();

    status = pdfWriter.EndPageContentContext(pageContent);
    if (status != eSuccess)
        return status;

    status = pdfWriter.WritePageAndRelease(page);
    if (status != eSuccess)
        return status;

    status = pdfWriter.EndPDF();
    if (status != eSuccess)
        return status;

    return status;
}

PDFHummus::EStatusCode MergeEmptyPageToForm(const std::string &inEmptyFileName)
{
    PDFWriter pdfWriter;
    EStatusCode status;
    PDFDocumentCopyingContext *copyingContext = nullptr;

    status =
        pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "MergeEmptyPageToForm.pdf"), ePDFVersion13);
    if (status != eSuccess)
        return status;

    // in this test we will merge 2 pages into a PDF form, and place it twice, forming a 2X2 design. amazing.

    PDFPage *page = new PDFPage();
    page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

    copyingContext = pdfWriter.CreatePDFCopyingContext(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, inEmptyFileName));
    if (status != eSuccess)
        return status;

    // create form for two pages.
    PDFFormXObject *newFormXObject = pdfWriter.StartFormXObject(PDFRectangle(0, 0, 297.5, 842));

    XObjectContentContext *xobjectContentContext = newFormXObject->GetContentContext();

    xobjectContentContext->q();
    xobjectContentContext->cm(0.5, 0, 0, 0.5, 0, 421);
    status = copyingContext->MergePDFPageToFormXObject(newFormXObject, 0);
    if (status != eSuccess)
        return status;
    xobjectContentContext->Q();

    ObjectIDType formID = newFormXObject->GetObjectID();
    status = pdfWriter.EndFormXObjectAndRelease(newFormXObject);
    if (status != eSuccess)
        return status;

    // now place it in the page
    PageContentContext *pageContentContext = pdfWriter.StartPageContentContext(page);

    std::string formName = page->GetResourcesDictionary().AddFormXObjectMapping(formID);

    pageContentContext->q();
    pageContentContext->Do(formName);
    pageContentContext->cm(1, 0, 0, 1, 297.5, 0);
    pageContentContext->Do(formName);
    pageContentContext->Q();

    status = pdfWriter.EndPageContentContext(pageContentContext);
    if (status != eSuccess)
        return status;

    status = pdfWriter.WritePageAndRelease(page);
    if (status != eSuccess)
        return status;

    status = pdfWriter.EndPDF();
    if (status != eSuccess)
        return status;

    delete copyingContext;

    return status;
}

TEST(PDFEmbedding, CopyingAndMergingEmptyPages)
{
    EStatusCode status = eSuccess;
    std::string emptyFileName = "SampleEmptyFileForCopying.pdf";

    status = PrepareSampleEmptyPDF(emptyFileName);
    ASSERT_EQ(status, eSuccess)
        << "CopyingAndMergingEmptyPages::Run, failed in creating sample empty file name for merging\n";

    status = CreateFormFromEmptyPage(emptyFileName);
    ASSERT_EQ(status, eSuccess) << "CopyingAndMergingEmptyPages::Run, failed in create form from an empty page test\n";

    status = CreatePageFromEmptyPage(emptyFileName);
    ASSERT_EQ(status, eSuccess) << "CopyingAndMergingEmptyPages::Run, failed in create form from an empty page test\n";

    status = MergeEmptyPageToPage(emptyFileName);
    ASSERT_EQ(status, eSuccess) << "CopyingAndMergingEmptyPages::Run, failed in create form from an empty page test\n";

    status = MergeEmptyPageToForm(emptyFileName);
    ASSERT_EQ(status, eSuccess) << "CopyingAndMergingEmptyPages::Run, failed in create form from an empty page test\n";
}