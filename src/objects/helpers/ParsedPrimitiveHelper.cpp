/*
 Source File : ParsedPrimitiveHelper.h


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

#include "objects/helpers/ParsedPrimitiveHelper.h"

#include "BoxingBase.h"
#include "objects/PDFBoolean.h"
#include "objects/PDFHexString.h"
#include "objects/PDFInteger.h"
#include "objects/PDFLiteralString.h"
#include "objects/PDFName.h"
#include "objects/PDFObject.h"
#include "objects/PDFReal.h"
#include "objects/PDFSymbol.h"
#include <utility>

ParsedPrimitiveHelper::ParsedPrimitiveHelper(std::shared_ptr<PDFObject> inObject)
{
    mWrappedObject = std::move(inObject);
}

double ParsedPrimitiveHelper::GetAsDouble()
{
    if (mWrappedObject->GetType() == PDFObject::ePDFObjectInteger)
    {
        auto anInteger = std::static_pointer_cast<PDFInteger>(mWrappedObject);
        return (double)anInteger->GetValue();
    }
    if (mWrappedObject->GetType() == PDFObject::ePDFObjectReal)
    {
        auto aReal = std::static_pointer_cast<PDFReal>(mWrappedObject);
        return aReal->GetValue();
    }
    return 0;
}

long long ParsedPrimitiveHelper::GetAsInteger()
{
    if (mWrappedObject->GetType() == PDFObject::ePDFObjectInteger)
    {
        auto anInteger = std::static_pointer_cast<PDFInteger>(mWrappedObject);
        return anInteger->GetValue();
    }
    if (mWrappedObject->GetType() == PDFObject::ePDFObjectReal)
    {
        auto aReal = std::static_pointer_cast<PDFReal>(mWrappedObject);
        return (long long)aReal->GetValue();
    }
    return 0;
}

bool ParsedPrimitiveHelper::IsNumber()
{
    return mWrappedObject->GetType() == PDFObject::ePDFObjectInteger ||
           mWrappedObject->GetType() == PDFObject::ePDFObjectReal;
}

typedef BoxingBaseWithRW<long long> LongLong;

// strings (relevant for: names, literal string, hex string, real, integer, symbol, boolean)
std::string ParsedPrimitiveHelper::ToString()
{
    std::string result;

    switch (mWrappedObject->GetType())
    {
    case PDFObject::ePDFObjectName:
        result = std::static_pointer_cast<PDFName>(mWrappedObject)->GetValue();
        break;
    case PDFObject::ePDFObjectLiteralString:
        result = std::static_pointer_cast<PDFLiteralString>(mWrappedObject)->GetValue();
        break;
    case PDFObject::ePDFObjectHexString:
        result = std::static_pointer_cast<PDFHexString>(mWrappedObject)->GetValue();
        break;
    case PDFObject::ePDFObjectReal:
        result = Double(std::static_pointer_cast<PDFReal>(mWrappedObject)->GetValue()).ToString();
        break;
    case PDFObject::ePDFObjectInteger:
        result = LongLong(std::static_pointer_cast<PDFInteger>(mWrappedObject)->GetValue()).ToString();
        break;
    case PDFObject::ePDFObjectSymbol:
        result = std::static_pointer_cast<PDFSymbol>(mWrappedObject)->GetValue();
        break;
    case PDFObject::ePDFObjectBoolean:
        result = std::static_pointer_cast<PDFBoolean>(mWrappedObject)->GetValue() ? "true" : "false";
        break;
    default:
        result = PDFObject::scPDFObjectTypeLabel(mWrappedObject->GetType());
    }
    return result;
}
