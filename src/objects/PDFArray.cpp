/*
   Source File : PDFArray.cpp


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
#include "PDFArray.h"

PDFArray::PDFArray() : PDFObject(eType)
{
}

PDFArray::~PDFArray()
{
    auto it = mValues.begin();

    for (; it != mValues.end(); ++it)
        (*it)->Release();
}

void PDFArray::AppendObject(PDFObject *inObject)
{
    inObject->AddRef();
    mValues.push_back(inObject);
}

SingleValueContainerIterator<PDFObjectVector> PDFArray::GetIterator()
{
    return SingleValueContainerIterator<PDFObjectVector>(mValues);
}

PDFObject *PDFArray::QueryObject(unsigned long i)
{
    if (mValues.size() <= i)
    {
        return nullptr;
    }
    else
    {
        mValues[i]->AddRef();
        return mValues[i];
    }
}

unsigned long PDFArray::GetLength()
{
    return (unsigned long)mValues.size();
}