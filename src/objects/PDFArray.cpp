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
#include "objects/PDFArray.h"

charta::PDFArray::PDFArray() : PDFObject(eType)
{
}

void charta::PDFArray::AppendObject(const std::shared_ptr<PDFObject> &inObject)
{
    mValues.push_back(inObject);
}

SingleValueContainerIterator<std::vector<std::shared_ptr<PDFObject>>> charta::PDFArray::GetIterator()
{
    return {mValues};
}

std::shared_ptr<PDFObject> charta::PDFArray::QueryObject(unsigned long i)
{
    if (mValues.size() <= i)
    {
        return nullptr;
    }

    return mValues[i];
}

unsigned long charta::PDFArray::GetLength()
{
    return (unsigned long)mValues.size();
}