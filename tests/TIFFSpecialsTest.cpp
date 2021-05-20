/*
   Source File : TiffSpecialsTest.cpp


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

#ifndef PDFHUMMUS_NO_TIFF

#include "PDFFormXObject.h"
#include "PDFPage.h"
#include "PDFWriter.h"
#include "PageContentContext.h"
#include "PagePresets.h"
#include "TestHelper.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

EStatusCode CreateBiLevelGrayScales(PDFWriter &inpdfWriter)
{
    EStatusCode status = PDFHummus::eSuccess;
    TIFFUsageParameters TIFFParameters;

    std::string scWJim = RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/jim___cg.tif");

    // GrayScale regular
    PDFFormXObject *imageGrayScale = inpdfWriter.CreateFormXObjectFromTIFFFile(scWJim);
    if (imageGrayScale == nullptr)
        return PDFHummus::eFailure;

    // GrayScale Green
    TIFFParameters.GrayscaleTreatment.AsColorMap = true;
    TIFFParameters.GrayscaleTreatment.OneColor = CMYKRGBColor(0, 255, 0);
    TIFFParameters.GrayscaleTreatment.ZeroColor = CMYKRGBColor(255, 255, 255);
    PDFFormXObject *imageGrayScaleGreen = inpdfWriter.CreateFormXObjectFromTIFFFile(scWJim, TIFFParameters);
    if (imageGrayScaleGreen == nullptr)
        return PDFHummus::eFailure;

    // grayscale cyan magenta
    TIFFParameters.GrayscaleTreatment.AsColorMap = true;
    TIFFParameters.GrayscaleTreatment.OneColor = CMYKRGBColor(255, 255, 0, 0);
    TIFFParameters.GrayscaleTreatment.ZeroColor = CMYKRGBColor(0, 0, 0, 0);
    PDFFormXObject *imageGrayScaleCyanMagenta = inpdfWriter.CreateFormXObjectFromTIFFFile(scWJim, TIFFParameters);
    if (imageGrayScaleCyanMagenta == nullptr)
        return PDFHummus::eFailure;

    // grayscale green vs red
    TIFFParameters.GrayscaleTreatment.AsColorMap = true;
    TIFFParameters.GrayscaleTreatment.OneColor = CMYKRGBColor(0, 255, 0);
    TIFFParameters.GrayscaleTreatment.ZeroColor = CMYKRGBColor(255, 0, 0);
    PDFFormXObject *imageGrayScaleGreenVSRed = inpdfWriter.CreateFormXObjectFromTIFFFile(scWJim, TIFFParameters);
    if (imageGrayScaleGreenVSRed == nullptr)
        return PDFHummus::eFailure;

    // grayscale cyan vs magenta
    TIFFParameters.GrayscaleTreatment.AsColorMap = true;
    TIFFParameters.GrayscaleTreatment.OneColor = CMYKRGBColor(255, 0, 0, 0);
    TIFFParameters.GrayscaleTreatment.ZeroColor = CMYKRGBColor(0, 255, 0, 0);
    PDFFormXObject *imageGrayScaleCyanVSMagenta = inpdfWriter.CreateFormXObjectFromTIFFFile(scWJim, TIFFParameters);
    if (imageGrayScaleCyanVSMagenta == nullptr)
        return PDFHummus::eFailure;

    // start page
    PDFPage page;
    page.SetMediaBox(PDFHummus::PagePresets::A4_Portrait);

    PageContentContext *pageContentContext = inpdfWriter.StartPageContentContext(page);
    if (pageContentContext == nullptr)
        return PDFHummus::eFailure;

    pageContentContext->q();
    pageContentContext->cm(1, 0, 0, 1, 0, 842 - 195.12);
    pageContentContext->Do(page.GetResourcesDictionary().AddFormXObjectMapping(imageGrayScale->GetObjectID()));
    pageContentContext->Q();

    pageContentContext->q();
    pageContentContext->cm(1, 0, 0, 1, 159.36, 842 - 195.12);
    pageContentContext->Do(page.GetResourcesDictionary().AddFormXObjectMapping(imageGrayScaleGreen->GetObjectID()));
    pageContentContext->Q();

    pageContentContext->q();
    pageContentContext->cm(1, 0, 0, 1, 159.36 * 2, 842 - 195.12);
    pageContentContext->Do(
        page.GetResourcesDictionary().AddFormXObjectMapping(imageGrayScaleCyanMagenta->GetObjectID()));
    pageContentContext->Q();

    pageContentContext->q();
    pageContentContext->cm(1, 0, 0, 1, 0, 842 - 195.12 * 2);
    pageContentContext->Do(
        page.GetResourcesDictionary().AddFormXObjectMapping(imageGrayScaleGreenVSRed->GetObjectID()));
    pageContentContext->Q();

    pageContentContext->q();
    pageContentContext->cm(1, 0, 0, 1, 159.36, 842 - 195.12 * 2);
    pageContentContext->Do(
        page.GetResourcesDictionary().AddFormXObjectMapping(imageGrayScaleCyanVSMagenta->GetObjectID()));
    pageContentContext->Q();

    delete imageGrayScale;
    delete imageGrayScaleGreen;
    delete imageGrayScaleCyanMagenta;
    delete imageGrayScaleGreenVSRed;
    delete imageGrayScaleCyanVSMagenta;

    status = inpdfWriter.EndPageContentContext(pageContentContext);
    if (status != PDFHummus::eSuccess)
        return status;

    return inpdfWriter.WritePage(page);
}

EStatusCode CreatePageForImageAndRelease(PDFWriter &inpdfWriter, PDFFormXObject *inImageObject)
{
    EStatusCode status = PDFHummus::eSuccess;
    PDFPage page;
    page.SetMediaBox(PDFHummus::PagePresets::A4_Portrait);

    PageContentContext *pageContentContext = inpdfWriter.StartPageContentContext(page);
    if (pageContentContext == nullptr)
        return PDFHummus::eFailure;

    std::string imageXObjectName = page.GetResourcesDictionary().AddFormXObjectMapping(inImageObject->GetObjectID());

    pageContentContext->q();
    pageContentContext->cm(1, 0, 0, 1, 0, 0);
    pageContentContext->Do(imageXObjectName);
    pageContentContext->Q();

    delete inImageObject;

    status = inpdfWriter.EndPageContentContext(pageContentContext);
    if (status != PDFHummus::eSuccess)
        return status;

    return inpdfWriter.WritePage(page);
}

EStatusCode CreateBlackAndWhiteMaskImage(PDFWriter &inpdfWriter)
{
    std::string scJimBW = RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/jim___ah.tif");
    EStatusCode status = PDFHummus::eSuccess;
    TIFFUsageParameters TIFFParameters;

    PDFFormXObject *imageBW = inpdfWriter.CreateFormXObjectFromTIFFFile(scJimBW);
    if (imageBW == nullptr)
        return PDFHummus::eFailure;

    TIFFParameters.BWTreatment.AsImageMask = true;
    TIFFParameters.BWTreatment.OneColor = CMYKRGBColor(255, 128, 0);
    PDFFormXObject *imageBWMask = inpdfWriter.CreateFormXObjectFromTIFFFile(scJimBW, TIFFParameters);
    if (imageBWMask == nullptr)
        return PDFHummus::eFailure;

    PDFPage page;
    page.SetMediaBox(PDFHummus::PagePresets::A4_Portrait);

    PageContentContext *pageContentContext = inpdfWriter.StartPageContentContext(page);
    if (pageContentContext == nullptr)
        return PDFHummus::eFailure;

    pageContentContext->q();
    pageContentContext->cm(1, 0, 0, 1, 0, 842 - 195.12);
    pageContentContext->Do(page.GetResourcesDictionary().AddFormXObjectMapping(imageBW->GetObjectID()));
    pageContentContext->Q();

    pageContentContext->q();
    pageContentContext->cm(1, 0, 0, 1, 159.36, 842 - 195.12);
    pageContentContext->rg(0, 0, 1);
    pageContentContext->re(0, 0, 159.36, 195.12);
    pageContentContext->f();
    pageContentContext->Do(page.GetResourcesDictionary().AddFormXObjectMapping(imageBWMask->GetObjectID()));
    pageContentContext->Q();

    delete imageBW;
    delete imageBWMask;

    status = inpdfWriter.EndPageContentContext(pageContentContext);
    if (status != PDFHummus::eSuccess)
        return status;

    return inpdfWriter.WritePage(page);
}

TEST(PDFImages, TIFFSpecials)
{
    PDFWriter pdfWriter;
    EStatusCode status;
    TIFFUsageParameters TIFFParameters;

    status = pdfWriter.StartPDF(
        RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "TiffSpecialsTest.pdf"), ePDFVersion13,
        LogConfiguration(true, true, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "TiffSpecialsTestLog.txt")));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // multipage Tiff
    for (int i = 0; i < 4; ++i)
    {
        TIFFParameters.PageIndex = i;
        PDFFormXObject *imageFormXObject = pdfWriter.CreateFormXObjectFromTIFFFile(
            RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/multipage.tif"), TIFFParameters);

        ASSERT_NE(imageFormXObject, nullptr);

        status = CreatePageForImageAndRelease(pdfWriter, imageFormXObject);
        ASSERT_EQ(status, PDFHummus::eSuccess);
    }

    // Black and White mask
    status = CreateBlackAndWhiteMaskImage(pdfWriter);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // Create BiLevel grayscales
    status = CreateBiLevelGrayScales(pdfWriter);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = pdfWriter.EndPDF();
    ASSERT_EQ(status, PDFHummus::eSuccess);
}

#endif