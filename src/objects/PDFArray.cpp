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

void PDFArray::AppendObject(std::shared_ptr<PDFObject> inObject)
{
    mValues.push_back(inObject);
}

SingleValueContainerIterator<PDFObjectVector> PDFArray::GetIterator()
{
    return SingleValueContainerIterator<PDFObjectVector>(mValues);
}

std::shared_ptr<PDFObject> PDFArray::QueryObject(unsigned long i)
{
    if (mValues.size() <= i)
    {
        return nullptr;
    }

    return mValues[i];
}

unsigned long PDFArray::GetLength()
{
    return (unsigned long)mValues.size();
}