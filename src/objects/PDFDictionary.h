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

struct PDFNameLess
{
    bool operator()(const PDFName *left, const PDFName *right) const
    {
        return left->GetValue() < right->GetValue();
    }
};

typedef std::map<PDFName *, PDFObject *, PDFNameLess> PDFNameToPDFObjectMap;

class PDFDictionary : public PDFObject
{
  public:
    enum EType
    {
        eType = ePDFObjectDictionary
    };

    PDFDictionary(void);
    virtual ~PDFDictionary(void);

    // AddRefs on both
    void Insert(PDFName *inKeyObject, PDFObject *inValueObject);

    bool Exists(const std::string& inName);
    PDFObject *QueryDirectObject(const std::string& inName);

    MapIterator<PDFNameToPDFObjectMap> GetIterator();

  private:
    PDFNameToPDFObjectMap mValues;
};
