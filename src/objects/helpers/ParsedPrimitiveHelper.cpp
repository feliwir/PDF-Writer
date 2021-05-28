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

ParsedPrimitiveHelper::ParsedPrimitiveHelper(std::shared_ptr<charta::PDFObject> inObject)
{
    mWrappedObject = std::move(inObject);
}

double ParsedPrimitiveHelper::GetAsDouble()
{
    if (mWrappedObject->GetType() == charta::PDFObject::ePDFObjectInteger)
    {
        auto anInteger = std::static_pointer_cast<charta::PDFInteger>(mWrappedObject);
        return (double)anInteger->GetValue();
    }
    if (mWrappedObject->GetType() == charta::PDFObject::ePDFObjectReal)
    {
        auto aReal = std::static_pointer_cast<charta::PDFReal>(mWrappedObject);
        return aReal->GetValue();
    }
    return 0;
}

long long ParsedPrimitiveHelper::GetAsInteger()
{
    if (mWrappedObject->GetType() == charta::PDFObject::ePDFObjectInteger)
    {
        auto anInteger = std::static_pointer_cast<charta::PDFInteger>(mWrappedObject);
        return anInteger->GetValue();
    }
    if (mWrappedObject->GetType() == charta::PDFObject::ePDFObjectReal)
    {
        auto aReal = std::static_pointer_cast<charta::PDFReal>(mWrappedObject);
        return (long long)aReal->GetValue();
    }
    return 0;
}

bool ParsedPrimitiveHelper::IsNumber()
{
    return mWrappedObject->GetType() == charta::PDFObject::ePDFObjectInteger ||
           mWrappedObject->GetType() == charta::PDFObject::ePDFObjectReal;
}

using LongLong = BoxingBaseWithRW<long long>;

// strings (relevant for: names, literal string, hex string, real, integer, symbol, boolean)
std::string ParsedPrimitiveHelper::ToString()
{
    std::string result;

    switch (mWrappedObject->GetType())
    {
    case charta::PDFObject::ePDFObjectName:
        result = std::static_pointer_cast<charta::PDFName>(mWrappedObject)->GetValue();
        break;
    case charta::PDFObject::ePDFObjectLiteralString:
        result = std::static_pointer_cast<charta::PDFLiteralString>(mWrappedObject)->GetValue();
        break;
    case charta::PDFObject::ePDFObjectHexString:
        result = std::static_pointer_cast<charta::PDFHexString>(mWrappedObject)->GetValue();
        break;
    case charta::PDFObject::ePDFObjectReal:
        result = Double(std::static_pointer_cast<charta::PDFReal>(mWrappedObject)->GetValue()).ToString();
        break;
    case charta::PDFObject::ePDFObjectInteger:
        result = LongLong(std::static_pointer_cast<charta::PDFInteger>(mWrappedObject)->GetValue()).ToString();
        break;
    case charta::PDFObject::ePDFObjectSymbol:
        result = std::static_pointer_cast<PDFSymbol>(mWrappedObject)->GetValue();
        break;
    case charta::PDFObject::ePDFObjectBoolean:
        result = std::static_pointer_cast<charta::PDFBoolean>(mWrappedObject)->GetValue() ? "true" : "false";
        break;
    default:
        result = charta::PDFObject::scPDFObjectTypeLabel(mWrappedObject->GetType());
    }
    return result;
}
