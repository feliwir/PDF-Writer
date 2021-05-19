/*
   Source File : TIFFImageTest.cpp


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

#include "BoxingBase.h"
#include "PDFFormXObject.h"
#include "PDFPage.h"
#include "PDFWriter.h"
#include "PageContentContext.h"
#include "TestHelper.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

EStatusCode AddPageForTIFF(PDFWriter &inpdfWriter, const std::string &inTiffFilePath)
{
    EStatusCode status = PDFHummus::eSuccess;

    PDFPage page;
    page.SetMediaBox(PDFHummus::PagePresets::A4_Portrait);

    PageContentContext *pageContentContext = inpdfWriter.StartPageContentContext(page);
    if (pageContentContext == nullptr)
        return PDFHummus::eFailure;

    PDFFormXObject *imageFormXObject = inpdfWriter.CreateFormXObjectFromTIFFFile(inTiffFilePath);
    if (imageFormXObject == nullptr)
        return PDFHummus::eFailure;

    std::string imageXObjectName = page.GetResourcesDictionary().AddFormXObjectMapping(imageFormXObject->GetObjectID());

    // continue page drawing, place the image in 0,0 (playing...could avoid CM at all)
    pageContentContext->q();
    pageContentContext->cm(1, 0, 0, 1, 0, 0);
    pageContentContext->Do(imageXObjectName);
    pageContentContext->Q();

    delete imageFormXObject;

    status = inpdfWriter.EndPageContentContext(pageContentContext);
    if (status != PDFHummus::eSuccess)
        return status;

    return inpdfWriter.WritePage(page);
}

TEST(PDFImages, TIFFImage)
{
    PDFWriter pdfWriter;
    EStatusCode status;

    status = pdfWriter.StartPDF(
        RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "BasicTIFFImagesTest.pdf"), ePDFVersion13,
        LogConfiguration(true, true, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "TiffImageTestLog.txt")));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    for (int i = 1; i < 9 && status != PDFHummus::eFailure; ++i)
        status = AddPageForTIFF(pdfWriter, std::string(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH,

                                                                              "data/images/tiff/CCITT_")) +

                                               Int(i).ToString() + ".TIF");
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/FLAG_T24.TIF"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/G4.TIF"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/G4S.TIF"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/G31D.TIF"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/G31DS.TIF"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/G32D.TIF"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/GMARBLES.TIF"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/MARBIBM.TIF"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/MARBLES.TIF"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/XING_T24.TIF"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // tiled image
    status =
        AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/cramps-tile.tif"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/cramps.tif"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // Looks corrupted by the tool. on mine looks good
    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/dscf0013.tif"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // Creates bad PDF by tool. on mine looks good
    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/fax2d.tif"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/g3test.tif"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/jello.tif"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/jim___ah.tif"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/jim___cg.tif"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/jim___dg.tif"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/jim___gg.tif"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/oxford.tif"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/quad-lzw.tif"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/quad-tile.tif"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/strike.tif"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/ycbcr-cat.tif"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    for (int i = 2; i < 9; i = i * 2)
    {
        status = AddPageForTIFF(pdfWriter, std::string(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH,
                                                                              "data/images/tiff/flower-minisblack-")) +
                                               Int(i).ToString() + ".tif");
        ASSERT_EQ(status, PDFHummus::eSuccess);
    }

    for (int i = 2; i < 9; i = i * 2)
    {
        status = AddPageForTIFF(
            pdfWriter, std::string(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/flower-palette-")) +
                           Int(i).ToString() + ".tif");
        ASSERT_EQ(status, PDFHummus::eSuccess);
    }

    for (int i = 2; i < 9; i = i * 2)
    {
        status = AddPageForTIFF(pdfWriter, std::string(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH,
                                                                              "data/images/tiff/flower-rgb-contig-")) +
                                               Int(i).ToString() + ".tif");
        ASSERT_EQ(status, PDFHummus::eSuccess);
    }

    status = AddPageForTIFF(pdfWriter,
                            RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/flower-rgb-planar-8.tif"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(
        pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/flower-separated-contig-8.tif"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = AddPageForTIFF(
        pdfWriter, RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/tiff/flower-separated-planar-8.tif"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = pdfWriter.EndPDF();
    ASSERT_EQ(status, PDFHummus::eSuccess);
}

#endif
