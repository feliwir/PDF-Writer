/*
 Source File : PDFPageInput.h


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

#include "objects/PDFPageInput.h"

#include "PagePresets.h"
#include "Trace.h"
#include "objects/PDFArray.h"
#include "objects/PDFName.h"
#include "objects/helpers/ParsedPrimitiveHelper.h"
#include "parsing/PDFParser.h"
#include <utility>

PDFPageInput::PDFPageInput(PDFParser *inParser, std::shared_ptr<PDFObject> inPageObject)
    : mPageObject(std::move(inPageObject))
{
    mParser = inParser;
    AssertPageObjectValid();
}

void PDFPageInput::AssertPageObjectValid()
{
    if (!mPageObject)
        TRACE_LOG("PDFPageInput::AssertPageObjectValid, null page object or not a dictionary");

    PDFObjectCastPtr<PDFName> typeObject = mPageObject->QueryDirectObject("Type");
    if (!typeObject || typeObject->GetValue() != "Page")
    {
        TRACE_LOG("PDFPageInput::AssertPageObjectValid, dictionar object provided is NOT a page object");
        mPageObject = nullptr;
    }
}

PDFPageInput::PDFPageInput(PDFParser *inParser, const PDFObjectCastPtr<PDFDictionary> &inPageObject)
{
    mParser = inParser;
    mPageObject = inPageObject;
    AssertPageObjectValid();
}

PDFPageInput::PDFPageInput(const PDFPageInput &inOtherPage)
{
    mParser = inOtherPage.mParser;
    mPageObject = inOtherPage.mPageObject;
    AssertPageObjectValid();
}

PDFPageInput::~PDFPageInput() = default;

bool PDFPageInput::operator!()
{
    return !mPageObject;
}

int PDFPageInput::GetRotate()
{
    int result = 0;
    std::shared_ptr<PDFObject> rotation(QueryInheritedValue(mPageObject, "Rotate"));
    if (!rotation)
        return result;

    ParsedPrimitiveHelper helper(rotation);
    if (!helper.IsNumber())
    {
        TRACE_LOG("PDFPageInput::GetRotate, Exception, pdf page rotation must be numeric value. defaulting to 0");
    }
    else
    {
        result = static_cast<int>(helper.GetAsInteger());
        if ((result % 90) != 0)
        {
            TRACE_LOG(
                "PDFPageInput::GetRotate, Exception, pdf page rotation must be a multiple of 90. defaulting to 0");
            result = 0;
        }
    }
    return result;
}

PDFRectangle PDFPageInput::GetMediaBox()
{
    PDFRectangle result;

    PDFObjectCastPtr<PDFArray> mediaBox(QueryInheritedValue(mPageObject, "MediaBox"));
    if (!mediaBox || mediaBox->GetLength() != 4)
    {
        TRACE_LOG("PDFPageInput::GetMediaBox, Exception, pdf page does not have correct media box. defaulting to A4");
        result = charta::PagePresets::A4_Portrait;
    }
    else
    {
        SetPDFRectangleFromPDFArray(mediaBox, result);
    }

    return result;
}

PDFRectangle PDFPageInput::GetCropBox()
{
    PDFRectangle result;
    PDFObjectCastPtr<PDFArray> cropBox(QueryInheritedValue(mPageObject, "CropBox"));

    if (!cropBox || cropBox->GetLength() != 4)
        result = GetMediaBox();
    else
        SetPDFRectangleFromPDFArray(cropBox, result);
    return result;
}

PDFRectangle PDFPageInput::GetTrimBox()
{
    return GetBoxAndDefaultWithCrop("TrimBox");
}

PDFRectangle PDFPageInput::GetBoxAndDefaultWithCrop(const std::string &inBoxName)
{
    PDFRectangle result;
    PDFObjectCastPtr<PDFArray> aBox(QueryInheritedValue(mPageObject, inBoxName));

    if (!aBox || aBox->GetLength() != 4)
        result = GetCropBox();
    else
        SetPDFRectangleFromPDFArray(aBox, result);
    return result;
}

PDFRectangle PDFPageInput::GetBleedBox()
{
    return GetBoxAndDefaultWithCrop("BleedBox");
}

PDFRectangle PDFPageInput::GetArtBox()
{
    return GetBoxAndDefaultWithCrop("ArtBox");
}

static const std::string scParent = "Parent";
std::shared_ptr<PDFObject> PDFPageInput::QueryInheritedValue(const std::shared_ptr<PDFDictionary> &inDictionary,
                                                             const std::string &inName)
{
    if (inDictionary->Exists(inName))
    {
        return mParser->QueryDictionaryObject(inDictionary, inName);
    }
    if (inDictionary->Exists(scParent))
    {
        PDFObjectCastPtr<PDFDictionary> parent(mParser->QueryDictionaryObject(inDictionary, scParent));
        if (!parent)
            return nullptr;
        return QueryInheritedValue(parent, inName);
    }
    return nullptr;
}

void PDFPageInput::SetPDFRectangleFromPDFArray(const std::shared_ptr<PDFArray> &inPDFArray,
                                               PDFRectangle &outPDFRectangle)
{
    std::shared_ptr<PDFObject> lowerLeftX(inPDFArray->QueryObject(0));
    std::shared_ptr<PDFObject> lowerLeftY(inPDFArray->QueryObject(1));
    std::shared_ptr<PDFObject> upperRightX(inPDFArray->QueryObject(2));
    std::shared_ptr<PDFObject> upperRightY(inPDFArray->QueryObject(3));

    outPDFRectangle.LowerLeftX = ParsedPrimitiveHelper(lowerLeftX).GetAsDouble();
    outPDFRectangle.LowerLeftY = ParsedPrimitiveHelper(lowerLeftY).GetAsDouble();
    outPDFRectangle.UpperRightX = ParsedPrimitiveHelper(upperRightX).GetAsDouble();
    outPDFRectangle.UpperRightY = ParsedPrimitiveHelper(upperRightY).GetAsDouble();
}
