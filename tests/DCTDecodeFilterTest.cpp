/*
 Source File : DCTDecodeFilterTest


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

#ifndef PDFHUMMUS_NO_DCT

#include "DictionaryContext.h"
#include "ObjectsContext.h"
#include "PDFFormXObject.h"
#include "PDFImageXObject.h"
#include "PDFPage.h"
#include "PDFStream.h"
#include "PDFWriter.h"
#include "PageContentContext.h"
#include "TestHelper.h"
#include "Trace.h"
#include "io/OutputStreamTraits.h"
#include "objects/PDFArray.h"
#include "objects/PDFDictionary.h"
#include "objects/PDFIndirectObjectReference.h"
#include "objects/PDFObject.h"
#include "objects/PDFObjectCast.h"
#include "objects/PDFStreamInput.h"
#include "parsing/PDFDocumentCopyingContext.h"
#include "parsing/PDFParser.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

EStatusCode CreateFileWithJPGImage(const std::string &inTestFileName)
{
    PDFWriter pdfWriter;
    EStatusCode status;

    status = pdfWriter.StartPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, inTestFileName), ePDFVersion13);
    if (status != PDFHummus::eSuccess)
        return status;

    PDFPage page;
    page.SetMediaBox(PDFHummus::PagePresets::A4_Portrait);

    PageContentContext *pageContentContext = pdfWriter.StartPageContentContext(page);
    if (pageContentContext == nullptr)
        return eFailure;

    // draw a rectangle
    pageContentContext->q();
    pageContentContext->k(100, 0, 0, 0);
    pageContentContext->re(500, 0, 100, 100);
    pageContentContext->f();
    pageContentContext->Q();

    // pause stream to start writing the image
    status = pdfWriter.PausePageContentContext(pageContentContext);
    if (status != PDFHummus::eSuccess)
        return status;

    // Create image xobject from
    PDFImageXObject *imageXObject = pdfWriter.CreateImageXObjectFromJPGFile(
        RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/images/otherStage.JPG"));
    if (imageXObject == nullptr)
        return eFailure;
    // continue page drawing size the image to 500,400
    pageContentContext->q();
    pageContentContext->cm(500, 0, 0, 400, 0, 0);
    pageContentContext->Do(page.GetResourcesDictionary().AddImageXObjectMapping(imageXObject));
    pageContentContext->Q();

    delete imageXObject;

    status = pdfWriter.EndPageContentContext(pageContentContext);
    if (status != PDFHummus::eSuccess)
        return status;
    status = pdfWriter.WritePage(page);
    if (status != PDFHummus::eSuccess)
        return status;
    status = pdfWriter.EndPDF();

    return status;
}

ObjectIDType FindDCTDecodedImageObject(PDFParser *inParser)
{
    ObjectIDType imageObject = 0;

    // find image by looking for the first image in the first page
    auto firstPage = inParser->ParsePage(0);
    if (!firstPage)
        return imageObject;

    PDFObjectCastPtr<PDFDictionary> resourceDictionary(
        inParser->QueryDictionaryObject(firstPage.GetPtr(), "Resources"));

    if (!resourceDictionary)
        return imageObject;

    PDFObjectCastPtr<PDFDictionary> xobjectDictionary(
        inParser->QueryDictionaryObject(resourceDictionary.GetPtr(), "XObject"));

    if (!xobjectDictionary)
        return imageObject;

    MapIterator<PDFNameToPDFObjectMap> it = xobjectDictionary->GetIterator();

    while (it.MoveNext())
    {
        if (it.GetValue()->GetType() == PDFObject::ePDFObjectIndirectObjectReference)
        {
            PDFObjectCastPtr<PDFStreamInput> image(
                inParser->ParseNewObject(((PDFIndirectObjectReference *)it.GetValue())->mObjectID));
            auto imageDictionary = image->QueryStreamDictionary();

            PDFObjectCastPtr<PDFName> objectType = imageDictionary->QueryDirectObject("Subtype");
            if (!objectType || objectType->GetValue() != "Image")
                continue;

            auto filters = imageDictionary->QueryDirectObject("Filter");
            if (!filters)
                break;

            if (filters->GetType() == PDFObject::ePDFObjectName &&
                ((PDFName *)filters.GetPtr())->GetValue() == "DCTDecode")
            {
                imageObject = ((PDFIndirectObjectReference *)it.GetValue())->mObjectID;
                break;
            }

            auto *filtersArray = (PDFArray *)filters.GetPtr();

            if (filtersArray->GetLength() == 1)
            {
                PDFObjectCastPtr<PDFName> firstDecoder(filtersArray->QueryObject(0));

                if (firstDecoder->GetValue() == "DCTDecode")
                {
                    imageObject = ((PDFIndirectObjectReference *)it.GetValue())->mObjectID;
                    break;
                }
            }
        }
    }

    return imageObject;
}

EStatusCode ModifyImageObject(PDFWriter *inWriter, ObjectIDType inImageObject)
{
    EStatusCode status = eSuccess;
    PDFDocumentCopyingContext *modifiedFileContext = inWriter->CreatePDFCopyingContextForModifiedFile();

    // get image source dictionary
    PDFObjectCastPtr<PDFStreamInput> imageStream(inWriter->GetModifiedFileParser().ParseNewObject(inImageObject));

    auto imageDictionary(imageStream->QueryStreamDictionary());

    // strt object for modified image
    inWriter->GetObjectsContext().StartModifiedIndirectObject(inImageObject);

    DictionaryContext *newImageDictionary = inWriter->GetObjectsContext().StartDictionary();

    MapIterator<PDFNameToPDFObjectMap> it = imageDictionary->GetIterator();

    // copy all but "Filter" and "Length"
    ObjectIDTypeList indirectObjects;

    while (it.MoveNext())
    {
        if (it.GetKey()->GetValue() == "Filter" || it.GetKey()->GetValue() == "Length")
            continue;

        newImageDictionary->WriteKey(it.GetKey()->GetValue());
        EStatusCodeAndObjectIDTypeList result = modifiedFileContext->CopyDirectObjectWithDeepCopy(it.GetValue());
        if (result.first != eSuccess)
        {
            status = result.first;
            break;
        }
        indirectObjects.insert(indirectObjects.end(), result.second.begin(), result.second.end());
    }
    if (status != PDFHummus::eSuccess)
        return status;

    // start image stream for this dictionary (make sure it's unfiltered)
    PDFStream *newImageStream = inWriter->GetObjectsContext().StartUnfilteredPDFStream(newImageDictionary);

    // copy source stream through read filter
    IByteReader *sourceImage =
        modifiedFileContext->GetSourceDocumentParser()->StartReadingFromStream(imageStream);
    if (sourceImage == nullptr)
        return eFailure;

    OutputStreamTraits traits(newImageStream->GetWriteStream());
    status = traits.CopyToOutputStream(sourceImage);

    // finalize stream
    inWriter->GetObjectsContext().EndPDFStream(newImageStream);
    delete newImageStream;
    delete sourceImage;

    // late check for status so stream is deleted
    if (status != PDFHummus::eSuccess)
        return status;

    // copy remaining indirect objects from image stream dictionary
    status = modifiedFileContext->CopyNewObjectsForDirectObject(indirectObjects);

    delete modifiedFileContext;

    return status;
}

TEST(DCTDecode, DCTDecodeFilter)
{
    EStatusCode status = eSuccess;

    /*
        what we're going to do here is this:

        1. create a PDF file using a jpg image. just like the regular jpg image test
        2. open the file for modification
        3. the jpg image should be using DCT decoder. make sure
        4. create a modified image object with the DCT decoding decompressed.
            4.1 start a new stream
            4.2 copy all but length and decoders details from the original image dictionary
            4.3 create a stream reader for the old image that will decompress the image
            4.4 write it as is to the new image object
        5. now you should have a PDF that has the image decompressed. if it's showing fine...then
           you got a working DCT decoder
     */

    const auto *testFileName = "DCTTest.pdf";

    // ceate file
    status = CreateFileWithJPGImage(testFileName);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // open file for modification
    PDFWriter pdfWriter;

    status = pdfWriter.ModifyPDF(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, testFileName), ePDFVersion13,
                                 RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, std::string("mod") + testFileName));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    ObjectIDType imageObject = FindDCTDecodedImageObject(&pdfWriter.GetModifiedFileParser());
    ASSERT_NE(imageObject, 0);

    status = ModifyImageObject(&pdfWriter, imageObject);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = pdfWriter.EndPDF();
    ASSERT_EQ(status, PDFHummus::eSuccess);
}

#endif