/*
   Source File : TiledPatternContentContext.cpp


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
#include "TiledPatternContentContext.h"
#include "DocumentContext.h"
#include "ITiledPatternEndWritingTask.h"
#include "PDFTiledPattern.h"

using namespace charta;

TiledPatternContentContext::TiledPatternContentContext(charta::DocumentContext *inDocumentContext,
                                                       PDFTiledPattern *inObject)
    : AbstractContentContext(inDocumentContext)
{
    mObjectOfContext = inObject;
    SetPDFStreamForWrite(mObjectOfContext->GetContentStream());
}

TiledPatternContentContext::~TiledPatternContentContext() = default;

ResourcesDictionary *TiledPatternContentContext::GetResourcesDictionary()
{
    return &(mObjectOfContext->GetResourcesDictionary());
}

class TiledPatternImageWritingTask : public ITiledPatternEndWritingTask
{
  public:
    TiledPatternImageWritingTask(const std::string &inImagePath, unsigned long inImageIndex, ObjectIDType inObjectID,
                                 const PDFParsingOptions &inPDFParsingOptions)
    {
        mImagePath = inImagePath;
        mImageIndex = inImageIndex;
        mObjectID = inObjectID;
        mPDFParsingOptions = inPDFParsingOptions;
    }

    ~TiledPatternImageWritingTask() override = default;

    charta::EStatusCode Write(PDFTiledPattern * /*inFormXObject*/, ObjectsContext * /*inObjectsContext*/,
                              charta::DocumentContext *inDocumentContext) override
    {
        return inDocumentContext->WriteFormForImage(mImagePath, mImageIndex, mObjectID, mPDFParsingOptions);
    }

  private:
    std::string mImagePath;
    unsigned long mImageIndex;
    ObjectIDType mObjectID;
    PDFParsingOptions mPDFParsingOptions;
};

void TiledPatternContentContext::ScheduleImageWrite(const std::string &inImagePath, unsigned long inImageIndex,
                                                    ObjectIDType inObjectID, const PDFParsingOptions &inParsingOptions)
{
    mDocumentContext->RegisterTiledPatternEndWritingTask(
        mObjectOfContext, new TiledPatternImageWritingTask(inImagePath, inImageIndex, inObjectID, inParsingOptions));
}