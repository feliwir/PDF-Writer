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

charta::PDFPageInput::PDFPageInput(PDFParser *inParser, std::shared_ptr<charta::PDFObject> inPageObject)
    : mPageObject(std::move(inPageObject))
{
    mParser = inParser;
    AssertPageObjectValid();
}

void charta::PDFPageInput::AssertPageObjectValid()
{
    if (!mPageObject)
        TRACE_LOG("charta::PDFPageInput::AssertPageObjectValid, null page object or not a dictionary");

    PDFObjectCastPtr<charta::PDFName> typeObject = mPageObject->QueryDirectObject("Type");
    if (!typeObject || typeObject->GetValue() != "Page")
    {
        TRACE_LOG("charta::PDFPageInput::AssertPageObjectValid, dictionar object provided is NOT a page object");
        mPageObject = nullptr;
    }
}

charta::PDFPageInput::PDFPageInput(PDFParser *inParser, const PDFObjectCastPtr<charta::PDFDictionary> &inPageObject)
{
    mParser = inParser;
    mPageObject = inPageObject;
    AssertPageObjectValid();
}

charta::PDFPageInput::PDFPageInput(const PDFPageInput &inOtherPage)
{
    mParser = inOtherPage.mParser;
    mPageObject = inOtherPage.mPageObject;
    AssertPageObjectValid();
}

charta::PDFPageInput::~PDFPageInput() = default;

bool charta::PDFPageInput::operator!()
{
    return !mPageObject;
}

int charta::PDFPageInput::GetRotate()
{
    int result = 0;
    std::shared_ptr<charta::PDFObject> rotation(QueryInheritedValue(mPageObject, "Rotate"));
    if (!rotation)
        return result;

    ParsedPrimitiveHelper helper(rotation);
    if (!helper.IsNumber())
    {
        TRACE_LOG(
            "charta::PDFPageInput::GetRotate, Exception, pdf page rotation must be numeric value. defaulting to 0");
    }
    else
    {
        result = static_cast<int>(helper.GetAsInteger());
        if ((result % 90) != 0)
        {
            TRACE_LOG("charta::PDFPageInput::GetRotate, Exception, pdf page rotation must be a multiple of 90. "
                      "defaulting to 0");
            result = 0;
        }
    }
    return result;
}

PDFRectangle charta::PDFPageInput::GetMediaBox()
{
    PDFRectangle result;

    PDFObjectCastPtr<charta::PDFArray> mediaBox(QueryInheritedValue(mPageObject, "MediaBox"));
    if (!mediaBox || mediaBox->GetLength() != 4)
    {
        TRACE_LOG(
            "charta::PDFPageInput::GetMediaBox, Exception, pdf page does not have correct media box. defaulting to A4");
        result = charta::PagePresets::A4_Portrait;
    }
    else
    {
        SetPDFRectangleFromPDFArray(mediaBox, result);
    }

    return result;
}

PDFRectangle charta::PDFPageInput::GetCropBox()
{
    PDFRectangle result;
    PDFObjectCastPtr<charta::PDFArray> cropBox(QueryInheritedValue(mPageObject, "CropBox"));

    if (!cropBox || cropBox->GetLength() != 4)
        result = GetMediaBox();
    else
        SetPDFRectangleFromPDFArray(cropBox, result);
    return result;
}

PDFRectangle charta::PDFPageInput::GetTrimBox()
{
    return GetBoxAndDefaultWithCrop("TrimBox");
}

PDFRectangle charta::PDFPageInput::GetBoxAndDefaultWithCrop(const std::string &inBoxName)
{
    PDFRectangle result;
    PDFObjectCastPtr<charta::PDFArray> aBox(QueryInheritedValue(mPageObject, inBoxName));

    if (!aBox || aBox->GetLength() != 4)
        result = GetCropBox();
    else
        SetPDFRectangleFromPDFArray(aBox, result);
    return result;
}

PDFRectangle charta::PDFPageInput::GetBleedBox()
{
    return GetBoxAndDefaultWithCrop("BleedBox");
}

PDFRectangle charta::PDFPageInput::GetArtBox()
{
    return GetBoxAndDefaultWithCrop("ArtBox");
}

static const std::string scParent = "Parent";
std::shared_ptr<charta::PDFObject> charta::PDFPageInput::QueryInheritedValue(
    const std::shared_ptr<charta::PDFDictionary> &inDictionary, const std::string &inName)
{
    if (inDictionary->Exists(inName))
    {
        return mParser->QueryDictionaryObject(inDictionary, inName);
    }
    if (inDictionary->Exists(scParent))
    {
        PDFObjectCastPtr<charta::PDFDictionary> parent(mParser->QueryDictionaryObject(inDictionary, scParent));
        if (!parent)
            return nullptr;
        return QueryInheritedValue(parent, inName);
    }
    return nullptr;
}

void charta::PDFPageInput::SetPDFRectangleFromPDFArray(const std::shared_ptr<charta::PDFArray> &inPDFArray,
                                                       PDFRectangle &outPDFRectangle)
{
    std::shared_ptr<charta::PDFObject> lowerLeftX(inPDFArray->QueryObject(0));
    std::shared_ptr<charta::PDFObject> lowerLeftY(inPDFArray->QueryObject(1));
    std::shared_ptr<charta::PDFObject> upperRightX(inPDFArray->QueryObject(2));
    std::shared_ptr<charta::PDFObject> upperRightY(inPDFArray->QueryObject(3));

    outPDFRectangle.LowerLeftX = ParsedPrimitiveHelper(lowerLeftX).GetAsDouble();
    outPDFRectangle.LowerLeftY = ParsedPrimitiveHelper(lowerLeftY).GetAsDouble();
    outPDFRectangle.UpperRightX = ParsedPrimitiveHelper(upperRightX).GetAsDouble();
    outPDFRectangle.UpperRightY = ParsedPrimitiveHelper(upperRightY).GetAsDouble();
}
