/*
   Source File : PDFDictionary.h


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
#pragma once
#include "MapIterator.h"
#include "PDFName.h"
#include "PDFObject.h"

#include <map>
#include <memory>

namespace charta
{
struct PDFNameLess
{
    bool operator()(const std::shared_ptr<PDFName> left, const std::shared_ptr<PDFName> right) const
    {
        return left->GetValue() < right->GetValue();
    }
};

class PDFDictionary : public PDFObject
{
  public:
    enum EType
    {
        eType = ePDFObjectDictionary
    };

    PDFDictionary();

    // AddRefs on both
    void Insert(const std::shared_ptr<PDFName> &inKeyObject, const std::shared_ptr<PDFObject> &inValueObject);

    bool Exists(const std::string &inName);
    std::shared_ptr<PDFObject> QueryDirectObject(const std::string &inName);

    MapIterator<std::map<std::shared_ptr<PDFName>, std::shared_ptr<PDFObject>, PDFNameLess>> GetIterator();

  private:
    std::map<std::shared_ptr<PDFName>, std::shared_ptr<PDFObject>, PDFNameLess> mValues;
};
} // namespace charta