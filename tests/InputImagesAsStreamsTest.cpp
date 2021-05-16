#include "PDFFormXObject.h"
#include "PDFPage.h"
#include "PDFWriter.h"
#include "PageContentContext.h"
#include "TestHelper.h"
#include "io/InputFile.h"

#include <gtest/gtest.h>
#include <iostream>
#include <string>

using namespace PDFHummus;

TEST(CustomStreams, InputImagesAsStreams)
{
    // A minimal test to see if images as streams work. i'm using regular file streams, just to show the point
    // obviously this is quite a trivial case.

    PDFWriter pdfWriter;
    EStatusCode status;

    status = pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "ImagesInStreams.pdf"), ePDFVersion13);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    auto *page = new PDFPage();
    page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

    // JPG image

    InputFile jpgImage;

    status = jpgImage.OpenFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/otherStage.JPG"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    PDFFormXObject *formXObject = pdfWriter.CreateFormXObjectFromJPGStream(jpgImage.GetInputStream());
    ASSERT_NE(formXObject, nullptr);

    jpgImage.CloseFile();

    PageContentContext *pageContentContext = pdfWriter.StartPageContentContext(page);
    ASSERT_NE(pageContentContext, nullptr);

    pageContentContext->q();
    pageContentContext->cm(1, 0, 0, 1, 0, 400);
    pageContentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(formXObject->GetObjectID()));
    pageContentContext->Q();

    delete formXObject;

    status = pdfWriter.EndPageContentContext(pageContentContext);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = pdfWriter.WritePageAndRelease(page);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // TIFF image
#ifndef PDFHUMMUS_NO_TIFF
    page = new PDFPage();
    page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

    InputFile tiffFile;
    status = tiffFile.OpenFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/FLAG_T24.TIF"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    formXObject = pdfWriter.CreateFormXObjectFromTIFFStream(tiffFile.GetInputStream());
    ASSERT_NE(formXObject, nullptr);

    tiffFile.CloseFile();
    pageContentContext = pdfWriter.StartPageContentContext(page);
    ASSERT_NE(pageContentContext, nullptr);

    // continue page drawing, place the image in 0,0 (playing...could avoid CM at all)
    pageContentContext->q();
    pageContentContext->cm(1, 0, 0, 1, 0, 0);
    pageContentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(formXObject->GetObjectID()));
    pageContentContext->Q();

    delete formXObject;

    status = pdfWriter.EndPageContentContext(pageContentContext);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = pdfWriter.WritePageAndRelease(page);
    ASSERT_EQ(status, PDFHummus::eSuccess);

#endif

    // PDF

    InputFile pdfFile;

    status = pdfFile.OpenFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/Original.pdf"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = pdfWriter.AppendPDFPagesFromPDF(pdfFile.GetInputStream(), PDFPageRange()).first;
    ASSERT_EQ(status, PDFHummus::eSuccess);

    pdfFile.CloseFile();

    status = pdfWriter.EndPDF();
    ASSERT_EQ(status, PDFHummus::eSuccess);
}