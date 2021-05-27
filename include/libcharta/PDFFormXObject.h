/*
   Source File : PDFFormXObject.h


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

#include "ObjectsBasicTypes.h"
#include "ResourcesDictionary.h"
#include <memory>

namespace charta
{
class DocumentContext;
};

class PDFStream;
class XObjectContentContext;
class ObjectsContext;

class PDFFormXObject
{
  public:
    PDFFormXObject(charta::DocumentContext *inDocumentContext, ObjectIDType inFormXObjectID,
                   std::shared_ptr<PDFStream> inXObjectStream, ObjectIDType inFormXObjectResourcesDictionaryID);
    ~PDFFormXObject(void);

    ObjectIDType GetObjectID() const;
    ObjectIDType GetResourcesDictionaryObjectID() const;

    ResourcesDictionary &GetResourcesDictionary();
    XObjectContentContext *GetContentContext();
    std::shared_ptr<PDFStream> GetContentStream();

  private:
    ObjectIDType mXObjectID;
    ObjectIDType mResourcesDictionaryID;
    ResourcesDictionary mResources;
    std::shared_ptr<PDFStream> mContentStream;
    XObjectContentContext *mContentContext;
};
