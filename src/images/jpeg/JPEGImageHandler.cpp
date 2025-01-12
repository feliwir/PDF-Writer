/*
   Source File : JPEGImageHandler.cpp


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
#include "images/jpeg/JPEGImageHandler.h"
#include "DictionaryContext.h"
#include "DocumentContext.h"
#include "EStatusCode.h"
#include "IDocumentContextExtender.h"
#include "ObjectsContext.h"
#include "PDFFormXObject.h"
#include "PDFImageXObject.h"
#include "PDFStream.h"
#include "ProcsetResourcesConstants.h"
#include "Trace.h"
#include "XObjectContentContext.h"
#include "images/jpeg/JPEGImageHandler.h"
#include "images/jpeg/JPEGImageParser.h"
#include "io/InputFile.h"
#include "io/OutputStreamTraits.h"

charta::JPEGImageHandler::JPEGImageHandler()
{
    mObjectsContext = nullptr;
    mDocumentContext = nullptr;
}

void charta::JPEGImageHandler::Reset()
{
    mImagesInformationMap.clear();
    mExtenders.clear();
}

void charta::JPEGImageHandler::SetOperationsContexts(DocumentContext *inDocumentContext,
                                                     ObjectsContext *inObjectsContext)
{
    mObjectsContext = inObjectsContext;
    mDocumentContext = inDocumentContext;
}

PDFImageXObject *charta::JPEGImageHandler::CreateImageXObjectFromJPGFile(const std::string &inJPGFilePath,
                                                                         ObjectIDType inImageXObjectID)
{
    PDFImageXObject *imageXObject = nullptr;

    do
    {
        // retrieve image information
        BoolAndJPEGImageInformation imageInformationResult = RetrieveImageInformation(inJPGFilePath);
        if (!imageInformationResult.first)
        {
            TRACE_LOG1(
                "charta::JPEGImageHandler::CreateImageXObjectJPGFile, unable to retrieve image information for %s",
                inJPGFilePath.c_str());
            break;
        }

        // Write Image XObject
        imageXObject = CreateAndWriteImageXObjectFromJPGInformation(inJPGFilePath, inImageXObjectID,
                                                                    imageInformationResult.second);

    } while (false);

    return imageXObject;
}

static const std::string scType = "Type";
static const std::string scXObject = "XObject";
static const std::string scSubType = "Subtype";

static const std::string scImage = "Image";
static const std::string scWidth = "Width";
static const std::string scHeight = "Height";
static const std::string scColorSpace = "ColorSpace";
static const std::string scDeviceGray = "DeviceGray";
static const std::string scDeviceRGB = "DeviceRGB";
static const std::string scDeviceCMYK = "DeviceCMYK";
static const std::string scDecode = "Decode";
static const std::string scBitsPerComponent = "BitsPerComponent";
static const std::string scFilter = "Filter";
static const std::string scDCTDecode = "DCTDecode";
PDFImageXObject *charta::JPEGImageHandler::CreateAndWriteImageXObjectFromJPGInformation(
    const std::string &inJPGFilePath, ObjectIDType inImageXObjectID, const JPEGImageInformation &inJPGImageInformation)
{
    InputFile JPGFile;
    if (JPGFile.OpenFile(inJPGFilePath) != charta::eSuccess)
    {
        TRACE_LOG1("charta::JPEGImageHandler::CreateAndWriteImageXObjectFromJPGInformation. Unable to open JPG file "
                   "for reading, %s",
                   inJPGFilePath.c_str());
        return nullptr;
    }

    PDFImageXObject *imageXObject =
        CreateAndWriteImageXObjectFromJPGInformation(JPGFile.GetInputStream(), inImageXObjectID, inJPGImageInformation);

    JPGFile.CloseFile();

    return imageXObject;
}

PDFImageXObject *charta::JPEGImageHandler::CreateAndWriteImageXObjectFromJPGInformation(
    charta::IByteReaderWithPosition *inJPGImageStream, ObjectIDType inImageXObjectID,
    const JPEGImageInformation &inJPGImageInformation)
{
    PDFImageXObject *imageXObject = nullptr;
    EStatusCode status = charta::eSuccess;

    do
    {
        if (mObjectsContext == nullptr)
        {
            TRACE_LOG("charta::JPEGImageHandler::CreateAndWriteImageXObjectFromJPGInformation. Unexpected Error, "
                      "mObjectsContext not initialized with an objects context");
            break;
        }

        mObjectsContext->StartNewIndirectObject(inImageXObjectID);
        DictionaryContext *imageContext = mObjectsContext->StartDictionary();

        // type
        imageContext->WriteKey(scType);
        imageContext->WriteNameValue(scXObject);

        // subtype
        imageContext->WriteKey(scSubType);
        imageContext->WriteNameValue(scImage);

        // Width
        imageContext->WriteKey(scWidth);
        imageContext->WriteIntegerValue(inJPGImageInformation.SamplesWidth);

        // Height
        imageContext->WriteKey(scHeight);
        imageContext->WriteIntegerValue(inJPGImageInformation.SamplesHeight);

        // Bits Per Component
        imageContext->WriteKey(scBitsPerComponent);
        imageContext->WriteIntegerValue(8);

        // Color Space and Decode Array if necessary
        imageContext->WriteKey(scColorSpace);
        switch (inJPGImageInformation.ColorComponentsCount)
        {
        case 1:
            imageContext->WriteNameValue(scDeviceGray);
            break;
        case 3:
            imageContext->WriteNameValue(scDeviceRGB);
            break;
        case 4:
            imageContext->WriteNameValue(scDeviceCMYK);

            // Decode array
            imageContext->WriteKey(scDecode);
            mObjectsContext->StartArray();
            for (int i = 0; i < 4; ++i)
            {
                mObjectsContext->WriteDouble(1);
                mObjectsContext->WriteDouble(0);
            }
            mObjectsContext->EndArray();
            mObjectsContext->EndLine();
            break;

        default:
            TRACE_LOG1(
                "charta::JPEGImageHandler::CreateAndWriteImageXObjectFromJPGInformation, Unexpected Error, unfamilar "
                "color components count - %d",
                inJPGImageInformation.ColorComponentsCount);
            status = charta::eFailure;
            break;
        }
        if (status != charta::eSuccess)
            break;

        // Decoder - DCTDecode
        imageContext->WriteKey(scFilter);
        imageContext->WriteNameValue(scDCTDecode);

        auto it = mExtenders.begin();
        EStatusCode status = charta::eSuccess;
        for (; it != mExtenders.end() && charta::eSuccess == status; ++it)
        {
            if ((*it)->OnJPEGImageXObjectWrite(inImageXObjectID, imageContext, mObjectsContext, mDocumentContext,
                                               this) != charta::eSuccess)
            {
                TRACE_LOG("charta::JPEGImageHandler::CreateAndWriteImageXObjectFromJPGInformation, unexpected failure. "
                          "extender declared failure when writing image xobject.");
                status = charta::eFailure;
                break;
            }
        }
        if (status != charta::eSuccess)
            break;

        std::shared_ptr<PDFStream> imageStream = mObjectsContext->StartUnfilteredPDFStream(imageContext);

        OutputStreamTraits outputTraits(imageStream->GetWriteStream());
        status = outputTraits.CopyToOutputStream(inJPGImageStream);
        if (status != charta::eSuccess)
        {
            TRACE_LOG(
                "charta::JPEGImageHandler::CreateAndWriteImageXObjectFromJPGInformation. Unexpected Error, failed to "
                "copy jpg stream to output stream");

            break;
        }

        mObjectsContext->EndPDFStream(imageStream);

        imageXObject = new PDFImageXObject(
            inImageXObjectID, 1 == inJPGImageInformation.ColorComponentsCount ? KProcsetImageB : KProcsetImageC);
    } while (false);

    return imageXObject;
}

charta::BoolAndJPEGImageInformation charta::JPEGImageHandler::RetrieveImageInformation(
    charta::IByteReaderWithPosition *inJPGStream)
{
    BoolAndJPEGImageInformation imageInformationResult(false, mNullInformation);

    do
    {
        JPEGImageParser jpgImageParser;
        JPEGImageInformation imageInformation;

        EStatusCode status = jpgImageParser.Parse(inJPGStream, imageInformation);
        if (status != charta::eSuccess)
        {
            TRACE_LOG("charta::JPEGImageHandler::JPEGImageHandler. Failed to parse JPG stream");
            break;
        }

        imageInformationResult.first = true;
        imageInformationResult.second = imageInformation;
    } while (false);

    return imageInformationResult;
}

charta::BoolAndJPEGImageInformation charta::JPEGImageHandler::RetrieveImageInformation(const std::string &inJPGFilePath)
{
    BoolAndJPEGImageInformation imageInformationResult(false, mNullInformation);

    auto it = mImagesInformationMap.find(inJPGFilePath);
    if (it == mImagesInformationMap.end())
    {
        do
        {
            InputFile JPGFile;
            EStatusCode status = JPGFile.OpenFile(inJPGFilePath);
            if (status != charta::eSuccess)
            {
                TRACE_LOG1("charta::JPEGImageHandler::JPEGImageHandler. Unable to open JPG file for reading, %s",
                           inJPGFilePath.c_str());
                break;
            }

            JPEGImageParser jpgImageParser;
            JPEGImageInformation imageInformation;

            status = jpgImageParser.Parse(JPGFile.GetInputStream(), imageInformation);
            if (status != charta::eSuccess)
            {
                TRACE_LOG1("charta::JPEGImageHandler::JPEGImageHandler. Failed to parse JPG file, %s",
                           inJPGFilePath.c_str());
                break;
            }

            status = JPGFile.CloseFile();
            if (status != charta::eSuccess)
            {
                TRACE_LOG1("charta::JPEGImageHandler::JPEGImageHandler. Failed to close JPG file, %s",
                           inJPGFilePath.c_str());
                break;
            }

            mImagesInformationMap.insert(StringToJPEGImageInformationMap::value_type(inJPGFilePath, imageInformation));
            imageInformationResult.first = true;
            imageInformationResult.second = imageInformation;
        } while (false);
    }
    else
    {
        imageInformationResult.first = true;
        imageInformationResult.second = it->second;
    }

    return imageInformationResult;
}

void charta::JPEGImageHandler::AddDocumentContextExtender(IDocumentContextExtender *inExtender)
{
    mExtenders.insert(inExtender);
}

void charta::JPEGImageHandler::RemoveDocumentContextExtender(IDocumentContextExtender *inExtender)
{
    mExtenders.erase(inExtender);
}

PDFFormXObject *charta::JPEGImageHandler::CreateFormXObjectFromJPGFile(const std::string &inJPGFilePath,
                                                                       ObjectIDType inFormXObjectID)
{
    PDFImageXObject *imageXObject = nullptr;
    PDFFormXObject *imageFormXObject = nullptr;

    do
    {
        if (mObjectsContext == nullptr)
        {
            TRACE_LOG("charta::JPEGImageHandler::CreateFormXObjectFromJPGFile. Unexpected Error, mDocumentContex not "
                      "initialized with a document context");
            break;
        }

        // retrieve image information
        BoolAndJPEGImageInformation imageInformationResult = RetrieveImageInformation(inJPGFilePath);
        if (!imageInformationResult.first)
        {
            TRACE_LOG1(
                "charta::JPEGImageHandler::CreateFormXObjectFromJPGFile, unable to retrieve image information for %s",
                inJPGFilePath.c_str());
            break;
        }

        // Write Image XObject
        imageXObject = CreateAndWriteImageXObjectFromJPGInformation(
            inJPGFilePath, mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID(),
            imageInformationResult.second);
        if (imageXObject == nullptr)
        {
            TRACE_LOG1("charta::JPEGImageHandler::CreateFormXObjectFromJPGFile, unable to create image xobject for %s",
                       inJPGFilePath.c_str());
            break;
        }

        // Write Image form XObject
        imageFormXObject =
            CreateImageFormXObjectFromImageXObject(imageXObject, inFormXObjectID, imageInformationResult.second);
        if (imageFormXObject == nullptr)
        {
            TRACE_LOG1("charta::JPEGImageHandler::CreateFormXObjectFromJPGFile, unable to create form xobject for %s",
                       inJPGFilePath.c_str());
            break;
        }

    } while (false);

    delete imageXObject;
    return imageFormXObject;
}

PDFFormXObject *charta::JPEGImageHandler::CreateImageFormXObjectFromImageXObject(
    PDFImageXObject *inImageXObject, ObjectIDType inFormXObjectID, const JPEGImageInformation &inJPGImageInformation)
{
    PDFFormXObject *formXObject = nullptr;
    do
    {
        if (mObjectsContext == nullptr)
        {
            TRACE_LOG("charta::JPEGImageHandler::CreateImageFormXObjectFromImageXObject. Unexpected Error, "
                      "mDocumentContex not "
                      "initialized with a document context");
            break;
        }

        std::pair<double, double> dimensions = GetImageDimensions(inJPGImageInformation);

        formXObject = mDocumentContext->StartFormXObject(PDFRectangle(0, 0, dimensions.first, dimensions.second),
                                                         inFormXObjectID);
        XObjectContentContext *xobjectContentContext = formXObject->GetContentContext();

        xobjectContentContext->q();
        xobjectContentContext->cm(dimensions.first, 0, 0, dimensions.second, 0, 0);
        xobjectContentContext->Do(formXObject->GetResourcesDictionary().AddImageXObjectMapping(inImageXObject));
        xobjectContentContext->Q();

        EStatusCode status = mDocumentContext->EndFormXObjectNoRelease(formXObject);
        if (status != charta::eSuccess)
        {
            TRACE_LOG(
                "charta::JPEGImageHandler::CreateImageFormXObjectFromImageXObject. Unexpected Error, could not create "
                "form XObject for image");
            delete formXObject;
            formXObject = nullptr;
            break;
        }

    } while (false);
    return formXObject;
}

std::pair<double, double> charta::JPEGImageHandler::GetImageDimensions(
    const JPEGImageInformation &inJPGImageInformation)
{
    std::pair<double, double> returnResult(1, 1);

    do
    {
        // prefer JFIF determined resolution
        if (inJPGImageInformation.JFIFInformationExists)
        {
            double jfifXDensity = (0 == inJPGImageInformation.JFIFXDensity) ? 1 : inJPGImageInformation.JFIFXDensity;
            double jfifYDensity = (0 == inJPGImageInformation.JFIFYDensity) ? 1 : inJPGImageInformation.JFIFYDensity;

            switch (inJPGImageInformation.JFIFUnit)
            {
            case 1: // INCH
                returnResult.first = ((double)inJPGImageInformation.SamplesWidth / jfifXDensity) * 72.0;
                returnResult.second = ((double)inJPGImageInformation.SamplesHeight / jfifYDensity) * 72.0;
                break;
            case 2: // CM
                returnResult.first = ((double)inJPGImageInformation.SamplesWidth / jfifXDensity) * 72.0 / 2.54;
                returnResult.second = ((double)inJPGImageInformation.SamplesHeight / jfifYDensity) * 72.0 / 2.54;
                break;
            default: // 0 - aspect ratio
                returnResult.first = (double)inJPGImageInformation.SamplesWidth;
                returnResult.second = (double)inJPGImageInformation.SamplesHeight;
                break;
            }
            break;
        }

        // if no jfif try exif
        if (inJPGImageInformation.ExifInformationExists)
        {
            double exifXDensity = (0 == inJPGImageInformation.ExifXDensity) ? 1 : inJPGImageInformation.ExifXDensity;
            double exifYDensity = (0 == inJPGImageInformation.ExifYDensity) ? 1 : inJPGImageInformation.ExifYDensity;

            switch (inJPGImageInformation.ExifUnit)
            {
            case 1: // aspect ratio
                returnResult.first = (double)inJPGImageInformation.SamplesWidth;
                returnResult.second = (double)inJPGImageInformation.SamplesHeight;
                break;
            case 3: // CM
                returnResult.first = ((double)inJPGImageInformation.SamplesWidth / exifXDensity) * 72.0 / 2.54;
                returnResult.second = ((double)inJPGImageInformation.SamplesHeight / exifYDensity) * 72.0 / 2.54;
                break;
            default: // 2 - Inch
                returnResult.first = ((double)inJPGImageInformation.SamplesWidth / exifXDensity) * 72.0;
                returnResult.second = ((double)inJPGImageInformation.SamplesHeight / exifYDensity) * 72.0;
                break;
            }
            break;
        }

        // if no jfif, try photoshop
        if (inJPGImageInformation.PhotoshopInformationExists)
        {
            double photoshopXDensity =
                (0 == inJPGImageInformation.PhotoshopXDensity) ? 1 : inJPGImageInformation.PhotoshopXDensity;
            double photoshopYDensity =
                (0 == inJPGImageInformation.PhotoshopYDensity) ? 1 : inJPGImageInformation.PhotoshopYDensity;

            returnResult.first = ((double)inJPGImageInformation.SamplesWidth / photoshopXDensity) * 72.0;
            returnResult.second = ((double)inJPGImageInformation.SamplesHeight / photoshopYDensity) * 72.0;
            break;
        }

        // else aspect ratio
        returnResult.first = (double)inJPGImageInformation.SamplesWidth;
        returnResult.second = (double)inJPGImageInformation.SamplesHeight;
    } while (false);

    return returnResult;
}

PDFImageXObject *charta::JPEGImageHandler::CreateImageXObjectFromJPGFile(const std::string &inJPGFilePath)
{
    if (mObjectsContext == nullptr)
    {
        TRACE_LOG("charta::JPEGImageHandler::CreateImageXObjectFromJPGFile. Unexpected Error, mObjectsContext not "
                  "initialized "
                  "with an objects context");
        return nullptr;
    }

    return CreateImageXObjectFromJPGFile(inJPGFilePath,
                                         mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID());
}

PDFFormXObject *charta::JPEGImageHandler::CreateFormXObjectFromJPGFile(const std::string &inJPGFilePath)
{
    if (mObjectsContext == nullptr)
    {
        TRACE_LOG(
            "charta::JPEGImageHandler::CreateFormXObjectFromJPGFile. Unexpected Error, mObjectsContext not initialized "
            "with an objects context");
        return nullptr;
    }

    return CreateFormXObjectFromJPGFile(inJPGFilePath,
                                        mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID());
}

PDFImageXObject *charta::JPEGImageHandler::CreateImageXObjectFromJPGStream(charta::IByteReaderWithPosition *inJPGStream)
{
    if (mObjectsContext == nullptr)
    {
        TRACE_LOG("charta::JPEGImageHandler::CreateImageXObjectFromJPGStream. Unexpected Error, mObjectsContext not "
                  "initialized with an objects context");
        return nullptr;
    }

    return CreateImageXObjectFromJPGStream(inJPGStream,
                                           mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID());
}

PDFImageXObject *charta::JPEGImageHandler::CreateImageXObjectFromJPGStream(charta::IByteReaderWithPosition *inJPGStream,
                                                                           ObjectIDType inImageXObjectID)
{
    PDFImageXObject *imageXObject = nullptr;

    do
    {
        if (mObjectsContext == nullptr)
        {
            TRACE_LOG(
                "charta::JPEGImageHandler::CreateImageXObjectFromJPGStream. Unexpected Error, mDocumentContex not "
                "initialized with a document context");
            break;
        }

        JPEGImageParser jpgImageParser;
        JPEGImageInformation imageInformation;

        long long recordedPosition = inJPGStream->GetCurrentPosition();

        EStatusCode status = jpgImageParser.Parse(inJPGStream, imageInformation);
        if (status != charta::eSuccess)
        {
            TRACE_LOG("charta::JPEGImageHandler::CreateImageXObjectFromJPGStream. Failed to parse JPG stream");
            break;
        }

        // reset image position after parsing header, for later content copying
        inJPGStream->SetPosition(recordedPosition);

        imageXObject = CreateAndWriteImageXObjectFromJPGInformation(inJPGStream, inImageXObjectID, imageInformation);

    } while (false);

    return imageXObject;
}

PDFFormXObject *charta::JPEGImageHandler::CreateFormXObjectFromJPGStream(charta::IByteReaderWithPosition *inJPGStream)
{
    if (mObjectsContext == nullptr)
    {
        TRACE_LOG("charta::JPEGImageHandler::CreateFormXObjectFromJPGStream. Unexpected Error, mObjectsContext not "
                  "initialized "
                  "with an objects context");
        return nullptr;
    }

    return CreateFormXObjectFromJPGStream(inJPGStream,
                                          mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID());
}

PDFFormXObject *charta::JPEGImageHandler::CreateFormXObjectFromJPGStream(charta::IByteReaderWithPosition *inJPGStream,
                                                                         ObjectIDType inFormXObjectID)
{
    PDFFormXObject *imageFormXObject = nullptr;
    PDFImageXObject *imageXObject = nullptr;

    do
    {
        if (mObjectsContext == nullptr)
        {
            TRACE_LOG(
                "charta::JPEGImageHandler::CreateImageXObjectFromJPGStream. Unexpected Error, mDocumentContex not "
                "initialized with a document context");
            break;
        }

        JPEGImageParser jpgImageParser;
        JPEGImageInformation imageInformation;

        long long recordedPosition = inJPGStream->GetCurrentPosition();

        EStatusCode status = jpgImageParser.Parse(inJPGStream, imageInformation);
        if (status != charta::eSuccess)
        {
            TRACE_LOG("charta::JPEGImageHandler::CreateImageXObjectFromJPGStream. Failed to parse JPG stream");
            break;
        }

        // reset image position after parsing header, for later content copying
        inJPGStream->SetPosition(recordedPosition);

        imageXObject = CreateAndWriteImageXObjectFromJPGInformation(
            inJPGStream, mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID(), imageInformation);
        if (imageXObject == nullptr)
        {
            TRACE_LOG("charta::JPEGImageHandler::CreateFormXObjectFromJPGStream, unable to create image xobject");
            break;
        }

        // Write Image form XObject
        imageFormXObject = CreateImageFormXObjectFromImageXObject(imageXObject, inFormXObjectID, imageInformation);
        if (imageFormXObject == nullptr)
        {
            TRACE_LOG("charta::JPEGImageHandler::CreateImageXObjectFromJPGStream, unable to create form xobject");
            break;
        }

    } while (false);

    delete imageXObject;
    return imageFormXObject;
}

int charta::JPEGImageHandler::GetColorComponents(const JPEGImageInformation &inJPGImageInformation)
{
    return inJPGImageInformation.ColorComponentsCount;
}