/*
   Source File : MergePDFPages.cpp


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
#include "DocumentContextExtenderAdapter.h"
#include "PDFPage.h"
#include "PDFWriter.h"
#include "PageContentContext.h"
#include "TestHelper.h"
#include "parsing/PDFDocumentCopyingContext.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

EStatusCode TestOnlyMerge()
{
    PDFWriter pdfWriter;
    EStatusCode status;

    status = pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "TestOnlyMerge.pdf"), ePDFVersion13);
    if (status != PDFHummus::eSuccess)
        return status;

    PDFPage page;
    page.SetMediaBox(PDFRectangle(0, 0, 595, 842));

    PDFPageRange singePageRange;
    singePageRange.mType = PDFPageRange::eRangeTypeSpecific;
    singePageRange.mSpecificRanges.push_back(ULongAndULong(0, 0));

    status = pdfWriter.MergePDFPagesToPage(
        page, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/BasicTIFFImagesTest.pdf"), singePageRange);
    if (status != PDFHummus::eSuccess)
        return status;

    status = pdfWriter.WritePage(page);
    if (status != PDFHummus::eSuccess)
        return status;

    return pdfWriter.EndPDF();
}

EStatusCode TestPrefixGraphicsMerge()
{
    PDFWriter pdfWriter;
    EStatusCode status;

    status =
        pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "TestPrefixGraphicsMerge.pdf"), ePDFVersion13);
    if (status != PDFHummus::eSuccess)
        return status;

    PDFPage page;
    page.SetMediaBox(PDFRectangle(0, 0, 595, 842));

    PDFUsedFont *font = pdfWriter.GetFontForFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/arial.ttf"));
    if (font == nullptr)
        return PDFHummus::eFailure;

    PageContentContext *pageContent = pdfWriter.StartPageContentContext(page);

    pageContent->BT();
    pageContent->k(0, 0, 0, 1);
    pageContent->Tf(font, 30);
    pageContent->Tm(1, 0, 0, 1, 10, 600);
    status = pageContent->Tj("Testing file merge");
    if (status != PDFHummus::eSuccess)
        return status;
    pageContent->ET();

    pageContent->cm(0.5, 0, 0, 0.5, 0, 0);

    PDFPageRange singePageRange;
    singePageRange.mType = PDFPageRange::eRangeTypeSpecific;
    singePageRange.mSpecificRanges.push_back(ULongAndULong(0, 0));

    status = pdfWriter.MergePDFPagesToPage(
        page, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/BasicTIFFImagesTest.pdf"), singePageRange);
    if (status != PDFHummus::eSuccess)
        return status;

    status = pdfWriter.EndPageContentContext(pageContent);
    if (status != PDFHummus::eSuccess)
        return status;

    status = pdfWriter.WritePage(page);
    if (status != PDFHummus::eSuccess)
        return status;

    return pdfWriter.EndPDF();
}

EStatusCode TestSuffixGraphicsMerge()
{
    PDFWriter pdfWriter;
    EStatusCode status;

    status =
        pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "TestSuffixGraphicsMerge.pdf"), ePDFVersion13);
    if (status != PDFHummus::eSuccess)
        return status;

    PDFPage page;
    page.SetMediaBox(PDFRectangle(0, 0, 595, 842));

    PDFPageRange singePageRange;
    singePageRange.mType = PDFPageRange::eRangeTypeSpecific;
    singePageRange.mSpecificRanges.push_back(ULongAndULong(0, 0));

    status = pdfWriter.MergePDFPagesToPage(
        page, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/BasicTIFFImagesTest.pdf"), singePageRange);
    if (status != PDFHummus::eSuccess)
        return status;

    PDFUsedFont *font = pdfWriter.GetFontForFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/arial.ttf"));
    if (font == nullptr)
        return PDFHummus::eFailure;

    PageContentContext *pageContent = pdfWriter.StartPageContentContext(page);

    pageContent->BT();
    pageContent->k(0, 0, 0, 1);
    pageContent->Tf(font, 30);
    pageContent->Tm(1, 0, 0, 1, 10, 600);
    status = pageContent->Tj("Testing file merge");
    if (status != PDFHummus::eSuccess)
        return status;
    pageContent->ET();

    status = pdfWriter.EndPageContentContext(pageContent);
    if (status != PDFHummus::eSuccess)
        return status;

    status = pdfWriter.WritePage(page);
    if (status != PDFHummus::eSuccess)
        return status;

    return pdfWriter.EndPDF();
}

EStatusCode TestBothGraphicsMerge()
{
    PDFWriter pdfWriter;
    EStatusCode status;

    status =
        pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "TestBothGraphicsMerge.pdf"), ePDFVersion13);
    if (status != PDFHummus::eSuccess)
        return status;

    PDFPage page;
    page.SetMediaBox(PDFRectangle(0, 0, 595, 842));

    PDFUsedFont *font = pdfWriter.GetFontForFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/arial.ttf"));
    if (font == nullptr)
        return PDFHummus::eFailure;

    PageContentContext *pageContent = pdfWriter.StartPageContentContext(page);

    pageContent->BT();
    pageContent->k(0, 0, 0, 1);
    pageContent->Tf(font, 30);
    pageContent->Tm(1, 0, 0, 1, 10, 600);
    status = pageContent->Tj("Testing file merge");
    if (status != PDFHummus::eSuccess)
        return status;
    pageContent->ET();

    pageContent->q();
    pageContent->cm(0.5, 0, 0, 0.5, 0, 0);

    PDFPageRange singePageRange;
    singePageRange.mType = PDFPageRange::eRangeTypeSpecific;
    singePageRange.mSpecificRanges.push_back(ULongAndULong(0, 0));

    status = pdfWriter.MergePDFPagesToPage(
        page, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/BasicTIFFImagesTest.pdf"), singePageRange);
    if (status != PDFHummus::eSuccess)
        return status;

    pageContent->Q();

    pageContent->q();
    pageContent->cm(1, 0, 0, 1, 30, 500);
    pageContent->k(0, 100, 100, 0);
    pageContent->re(0, 0, 200, 100);
    pageContent->f();
    pageContent->Q();

    status = pdfWriter.EndPageContentContext(pageContent);
    if (status != PDFHummus::eSuccess)
        return status;

    status = pdfWriter.WritePage(page);
    if (status != PDFHummus::eSuccess)
        return status;

    return pdfWriter.EndPDF();
}

EStatusCode MergeTwoPageInSeparatePhases()
{
    PDFWriter pdfWriter;
    EStatusCode status;

    status = pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "MergeTwoPageInSeparatePhases.pdf"),
                                ePDFVersion13);
    if (status != PDFHummus::eSuccess)
        return status;

    PDFPage page;
    page.SetMediaBox(PDFRectangle(0, 0, 595, 842));

    PageContentContext *pageContent = pdfWriter.StartPageContentContext(page);

    pageContent->q();
    pageContent->cm(0.5, 0, 0, 0.5, 0, 0);

    PDFPageRange firstPageRange;
    firstPageRange.mType = PDFPageRange::eRangeTypeSpecific;
    firstPageRange.mSpecificRanges.push_back(ULongAndULong(0, 0));

    status = pdfWriter.MergePDFPagesToPage(
        page, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/BasicTIFFImagesTest.pdf"), firstPageRange);
    if (status != PDFHummus::eSuccess)
        return status;

    pageContent->Q();

    pageContent->q();
    pageContent->cm(0.5, 0, 0, 0.5, 0, 421);

    PDFPageRange secondPageRange;
    secondPageRange.mType = PDFPageRange::eRangeTypeSpecific;
    secondPageRange.mSpecificRanges.push_back(ULongAndULong(1, 1));

    status = pdfWriter.MergePDFPagesToPage(
        page, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/BasicTIFFImagesTest.pdf"), secondPageRange);
    if (status != PDFHummus::eSuccess)
        return status;

    pageContent->Q();

    status = pdfWriter.EndPageContentContext(pageContent);
    if (status != PDFHummus::eSuccess)
        return status;

    status = pdfWriter.WritePage(page);
    if (status != PDFHummus::eSuccess)
        return status;

    return pdfWriter.EndPDF();
}

// for the next trick i'll need an event handler, in order to write the code between the pages.
class MyPDFMergingHandler : public DocumentContextExtenderAdapter
{
  public:
    MyPDFMergingHandler(PageContentContext *inPageContentContext)
    {
        mPageContentContext = inPageContentContext;
        mPageIndex = 0;
    }

    EStatusCode OnAfterMergePageFromPage(PDFPage & /*inTargetPage*/, PDFDictionary * /*inPageObjectDictionary*/,
                                         ObjectsContext * /*inPDFWriterObjectContext*/,
                                         DocumentContext * /*inPDFWriterDocumentContext*/,
                                         PDFDocumentHandler * /*inPDFDocumentHandler*/) override
    {
        // implementing the after merge for page event to include the required code between the two pages

        if (0 == mPageIndex)
        {
            mPageContentContext->Q();
            mPageContentContext->q();
            mPageContentContext->cm(0.5, 0, 0, 0.5, 0, 421);
        }
        ++mPageIndex;
        return PDFHummus::eSuccess;
    }

  private:
    int mPageIndex;
    PageContentContext *mPageContentContext;
};

EStatusCode MergeTwoPageWithEvents()
{
    PDFWriter pdfWriter;
    EStatusCode status;

    status =
        pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "MergeTwoPageWithEvents.pdf"), ePDFVersion13);
    if (status != PDFHummus::eSuccess)
        return status;

    PDFPage page;
    page.SetMediaBox(PDFRectangle(0, 0, 595, 842));

    PageContentContext *pageContent = pdfWriter.StartPageContentContext(page);

    pageContent->q();
    pageContent->cm(0.5, 0, 0, 0.5, 0, 0);

    PDFPageRange twoPageRange;
    twoPageRange.mType = PDFPageRange::eRangeTypeSpecific;
    twoPageRange.mSpecificRanges.push_back(ULongAndULong(0, 1));

    MyPDFMergingHandler mergingHandler(pageContent);

    // i'm using events to write the interim code, as oppose to using two merges as TestBothGraphicsMerge shows.
    // this is more efficient, considering the embedded PDF. but still, the easiest would be to use the copying
    // context
    pdfWriter.GetDocumentContext().AddDocumentContextExtender(&mergingHandler);
    status = pdfWriter.MergePDFPagesToPage(
        page, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/BasicTIFFImagesTest.pdf"), twoPageRange);
    if (status != PDFHummus::eSuccess)
        return status;

    pdfWriter.GetDocumentContext().RemoveDocumentContextExtender(&mergingHandler);

    pageContent->Q();

    status = pdfWriter.EndPageContentContext(pageContent);
    if (status != PDFHummus::eSuccess)
        return status;

    status = pdfWriter.WritePage(page);
    if (status != PDFHummus::eSuccess)
        return status;

    return pdfWriter.EndPDF();
}

EStatusCode MergePagesUsingCopyingContext()
{
    // this is by far the best method if you want to merge multiple pages in a pdf - using the copying context
    PDFWriter pdfWriter;
    EStatusCode status;

    // in this sample we'll create two pages, from 3 pages content of the merged page.
    // the first page will be used as a reusable object in both result pages. the second and third page will
    // be reusables.
    status = pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "MergePagesUsingCopyingContext.pdf"),
                                ePDFVersion13);
    if (status != PDFHummus::eSuccess)
        return status;

    PDFDocumentCopyingContext *copyingContext =
        pdfWriter.CreatePDFCopyingContext(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/BasicTIFFImagesTest.pdf"));
    if (status != PDFHummus::eSuccess)
        return status;

    // create a reusable form xobject from the first page
    EStatusCodeAndObjectIDType result = copyingContext->CreateFormXObjectFromPDFPage(0, ePDFPageBoxMediaBox);
    if (result.first != PDFHummus::eSuccess)
        return result.first;

    ObjectIDType reusableObjectID = result.second;

    // now let's begin constructing the pages
    {
        PDFPage page;
        page.SetMediaBox(PDFRectangle(0, 0, 595, 842));

        PageContentContext *pageContent = pdfWriter.StartPageContentContext(page);

        pageContent->q();
        pageContent->cm(0.5, 0, 0, 0.5, 0, 0);

        // merge unique page at lower left
        status = copyingContext->MergePDFPageToPage(page, 1);
        if (status != PDFHummus::eSuccess)
            return status;

        pageContent->Q();

        pageContent->q();
        pageContent->cm(0.5, 0, 0, 0.5, 297.5, 421);

        // place reusable object page on the upper right corner of the page
        pageContent->Do(page.GetResourcesDictionary().AddFormXObjectMapping(reusableObjectID));

        pageContent->Q();

        status = pdfWriter.EndPageContentContext(pageContent);
        if (status != PDFHummus::eSuccess)
            return status;

        status = pdfWriter.WritePage(page);
        if (status != PDFHummus::eSuccess)
            return status;
    }

    // now let's do the second page. similar, but with the second page as the unique content
    {
        PDFPage page;
        page.SetMediaBox(PDFRectangle(0, 0, 595, 842));

        PageContentContext *pageContent = pdfWriter.StartPageContentContext(page);

        pageContent->q();
        pageContent->cm(0.5, 0, 0, 0.5, 0, 0);

        // merge unique page at lower left
        status = copyingContext->MergePDFPageToPage(page, 2);
        if (status != PDFHummus::eSuccess)
            return status;

        pageContent->Q();

        pageContent->q();
        pageContent->cm(0.5, 0, 0, 0.5, 297.5, 421);

        // place reusable object page on the upper right corner of the page
        pageContent->Do(page.GetResourcesDictionary().AddFormXObjectMapping(reusableObjectID));

        pageContent->Q();

        status = pdfWriter.EndPageContentContext(pageContent);
        if (status != PDFHummus::eSuccess)
            return status;

        status = pdfWriter.WritePage(page);
        if (status != PDFHummus::eSuccess)
            return status;
    }

    return pdfWriter.EndPDF();
}

TEST(PDFEmbedding, MergePDFPages)
{
    /*
    a. Regular (no copying context):
        a. Merging one page
            0. No graphics
            i. Some graphic before
            ii. Some graphic after
            iii. Both before and after
        b. Merging two pages
            i. In two phases
                1) Some graphics between them, and after the last one
            ii. In one phase, with events for some graphics between them
    b. Copying context:
        a. Simple, merging one page with some graphics before and after

    Importing two pages, merge one and user the other as xobject, with graphics between them
    */

    EXPECT_EQ(TestOnlyMerge(), PDFHummus::eSuccess);
    EXPECT_EQ(TestPrefixGraphicsMerge(), PDFHummus::eSuccess);
    EXPECT_EQ(TestSuffixGraphicsMerge(), PDFHummus::eSuccess);
    EXPECT_EQ(TestBothGraphicsMerge(), PDFHummus::eSuccess);
    EXPECT_EQ(MergeTwoPageInSeparatePhases(), PDFHummus::eSuccess);
    EXPECT_EQ(MergeTwoPageWithEvents(), PDFHummus::eSuccess);
    EXPECT_EQ(MergePagesUsingCopyingContext(), PDFHummus::eSuccess);
}