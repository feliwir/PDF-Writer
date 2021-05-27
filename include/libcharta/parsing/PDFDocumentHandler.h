/*
   Source File : PDFDocumentHandler.h


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

#include "DocumentContextExtenderAdapter.h"
#include "ETokenSeparator.h"
#include "MapIterator.h"
#include "ObjectsBasicTypes.h"
#include "PDFEmbedParameterTypes.h"
#include "PDFParser.h"
#include "PDFParsingOptions.h"
#include "PDFRectangle.h"
#include "io/InputFile.h"

#include <list>
#include <map>
#include <set>

class ObjectsContext;
class IByteWriter;
class PDFDictionary;
class PDFArray;
class PDFStreamInput;
class DictionaryContext;
class PageContentContext;
class PDFPage;
class IDocumentContextExtender;
class IPageEmbedInFormCommand;
class IPDFParserExtender;
class ICategoryServicesCommand;
class PDFIndirectObjectReference;
class PDFObject;

namespace charta
{
class DocumentContext;
}

using namespace charta;
typedef std::map<ObjectIDType, ObjectIDType> ObjectIDTypeToObjectIDTypeMap;
typedef std::map<std::string, std::string> StringToStringMap;
typedef std::set<ObjectIDType> ObjectIDTypeSet;
typedef std::set<IDocumentContextExtender *> IDocumentContextExtenderSet;

struct ResourceTokenMarker
{
    ResourceTokenMarker(std::string inResourceToken, long long inResourceTokenPosition)
    {
        ResourceToken = inResourceToken;
        ResourceTokenPosition = inResourceTokenPosition;
    }

    std::string ResourceToken;
    long long ResourceTokenPosition;
};

class IObjectWritePolicy
{
  public:
    virtual void WriteReference(std::shared_ptr<PDFIndirectObjectReference> inReference,
                                ETokenSeparator inSeparator) = 0;
};

class PDFDocumentHandler;

class InWritingPolicy : public IObjectWritePolicy
{
  public:
    InWritingPolicy(PDFDocumentHandler *inDocumentHandler)
    {
        mDocumentHandler = inDocumentHandler;
    }
    virtual void WriteReference(std::shared_ptr<PDFIndirectObjectReference> inReference, ETokenSeparator inSeparator);

  private:
    PDFDocumentHandler *mDocumentHandler;
};

class OutWritingPolicy : public IObjectWritePolicy
{
  public:
    OutWritingPolicy(PDFDocumentHandler *inDocumentHandler, ObjectIDTypeList &ioSourceObjectsToAdd)
        : mSourceObjectsToAdd(ioSourceObjectsToAdd)
    {
        mDocumentHandler = inDocumentHandler;
    }
    virtual void WriteReference(std::shared_ptr<PDFIndirectObjectReference> inReference, ETokenSeparator inSeparator);

  private:
    PDFDocumentHandler *mDocumentHandler;
    ObjectIDTypeList &mSourceObjectsToAdd;
};

typedef std::list<ResourceTokenMarker> ResourceTokenMarkerList;

class PDFDocumentHandler : public DocumentContextExtenderAdapter
{
    friend class InWritingPolicy;
    friend class OutWritingPolicy;

  public:
    PDFDocumentHandler(void);
    virtual ~PDFDocumentHandler(void);

    void SetOperationsContexts(charta::DocumentContext *inDocumentContext, ObjectsContext *inObjectsContext);

    // Create a list of XObjects from a PDF file.
    // the list of objects can then be used to place the "pages" in various locations on the written
    // PDF page.
    EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(
        const std::string &inPDFFilePath, const PDFParsingOptions &inParsingOptions, const PDFPageRange &inPageRange,
        EPDFPageBox inPageBoxToUseAsFormBox, const double *inTransformationMatrix,
        const ObjectIDTypeList &inCopyAdditionalObjects, const ObjectIDTypeList &inPredefinedFormIDs);

    EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(
        IByteReaderWithPosition *inPDFStream, const PDFParsingOptions &inParsingOptions,
        const PDFPageRange &inPageRange, EPDFPageBox inPageBoxToUseAsFormBox, const double *inTransformationMatrix,
        const ObjectIDTypeList &inCopyAdditionalObjects, const ObjectIDTypeList &inPredefinedFormIDs);

    EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(
        const std::string &inPDFFilePath, const PDFParsingOptions &inParsingOptions, const PDFPageRange &inPageRange,
        const PDFRectangle &inCropBox, const double *inTransformationMatrix,
        const ObjectIDTypeList &inCopyAdditionalObjects, const ObjectIDTypeList &inPredefinedFormIDs);

    EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(
        IByteReaderWithPosition *inPDFStream, const PDFParsingOptions &inParsingOptions,
        const PDFPageRange &inPageRange, const PDFRectangle &inCropBox, const double *inTransformationMatrix,
        const ObjectIDTypeList &inCopyAdditionalObjects, const ObjectIDTypeList &inPredefinedFormIDs);

    // appends pages from source PDF to the written PDF. returns object ID for the created pages
    EStatusCodeAndObjectIDTypeList AppendPDFPagesFromPDF(const std::string &inPDFFilePath,
                                                         const PDFParsingOptions &inParsingOptions,
                                                         const PDFPageRange &inPageRange,
                                                         const ObjectIDTypeList &inCopyAdditionalObjects);

    EStatusCodeAndObjectIDTypeList AppendPDFPagesFromPDF(IByteReaderWithPosition *inPDFStream,
                                                         const PDFParsingOptions &inParsingOptions,
                                                         const PDFPageRange &inPageRange,
                                                         const ObjectIDTypeList &inCopyAdditionalObjects);

    // MergePDFPagesToPage, merge PDF pages content to an input page. good for single-placement of a page content,
    // cheaper than creating and XObject and later placing, when the intention is to use this graphic just once.
    charta::EStatusCode MergePDFPagesToPage(PDFPage &inPage, const std::string &inPDFFilePath,
                                            const PDFParsingOptions &inParsingOptions, const PDFPageRange &inPageRange,
                                            const ObjectIDTypeList &inCopyAdditionalObjects);

    charta::EStatusCode MergePDFPagesToPage(PDFPage &inPage, IByteReaderWithPosition *inPDFStream,
                                            const PDFParsingOptions &inParsingOptions, const PDFPageRange &inPageRange,
                                            const ObjectIDTypeList &inCopyAdditionalObjects);

    // Event listeners for CreateFormXObjectsFromPDF and AppendPDFPagesFromPDF
    void AddDocumentContextExtender(IDocumentContextExtender *inExtender);
    void RemoveDocumentContextExtender(IDocumentContextExtender *inExtender);

    // IDocumentContextExtender implementation
    virtual charta::EStatusCode OnResourcesWrite(ResourcesDictionary *inResources,
                                                 DictionaryContext *inPageResourcesDictionaryContext,
                                                 ObjectsContext *inPDFWriterObjectContext,
                                                 charta::DocumentContext *inPDFWriterDocumentContext);

    // copying context handling
    charta::EStatusCode StartFileCopyingContext(const std::string &inPDFFilePath, const PDFParsingOptions &inOptions);
    charta::EStatusCode StartStreamCopyingContext(IByteReaderWithPosition *inPDFStream,
                                                  const PDFParsingOptions &inOptions);
    charta::EStatusCode StartParserCopyingContext(PDFParser *inPDFParser);
    EStatusCodeAndObjectIDType CreateFormXObjectFromPDFPage(unsigned long inPageIndex,
                                                            EPDFPageBox inPageBoxToUseAsFormBox,
                                                            const double *inTransformationMatrix,
                                                            ObjectIDType inPredefinedFormId);
    EStatusCodeAndObjectIDType CreateFormXObjectFromPDFPage(unsigned long inPageIndex, const PDFRectangle &inCropBox,
                                                            const double *inTransformationMatrix,
                                                            ObjectIDType inPredefinedFormId);
    EStatusCodeAndObjectIDType AppendPDFPageFromPDF(unsigned long inPageIndex);
    charta::EStatusCode MergePDFPageToPage(PDFPage &inTargetPage, unsigned long inSourcePageIndex);
    charta::EStatusCode MergePDFPageToFormXObject(PDFFormXObject *inTargetFormXObject, unsigned long inSourcePageIndex);
    EStatusCodeAndObjectIDType CopyObject(ObjectIDType inSourceObjectID);
    PDFParser *GetSourceDocumentParser();
    EStatusCodeAndObjectIDType GetCopiedObjectID(ObjectIDType inSourceObjectID);
    MapIterator<ObjectIDTypeToObjectIDTypeMap> GetCopiedObjectsMappingIterator();
    EStatusCodeAndObjectIDTypeList CopyDirectObjectWithDeepCopy(std::shared_ptr<PDFObject> inObject);
    charta::EStatusCode CopyDirectObjectAsIs(std::shared_ptr<PDFObject> inObject);
    charta::EStatusCode CopyNewObjectsForDirectObject(const ObjectIDTypeList &inReferencedObjects);
    void StopCopyingContext();
    void ReplaceSourceObjects(const ObjectIDTypeToObjectIDTypeMap &inSourceObjectsToNewTargetObjects);
    IByteReaderWithPosition *GetSourceDocumentStream();

    // Internal implementation. do not use directly
    PDFFormXObject *CreatePDFFormXObjectForPage(unsigned long inPageIndex, EPDFPageBox inPageBoxToUseAsFormBox,
                                                const double *inTransformationMatrix,
                                                ObjectIDType inPredefinedObjectId);
    PDFFormXObject *CreatePDFFormXObjectForPage(unsigned long inPageIndex, const PDFRectangle &inCropBox,
                                                const double *inTransformationMatrix,
                                                ObjectIDType inPredefinedObjectId);
    void RegisterFormRelatedObjects(PDFFormXObject *inFormXObject, const ObjectIDTypeList &inObjectsToWrite);

    // Extendibility
    void SetParserExtender(IPDFParserExtender *inParserExtender);

  private:
    ObjectsContext *mObjectsContext;
    charta::DocumentContext *mDocumentContext;
    IDocumentContextExtenderSet mExtenders;

    InputFile mPDFFile;
    IByteReaderWithPosition *mPDFStream;
    PDFParser *mParser;
    bool mParserOwned;
    ObjectIDTypeToObjectIDTypeMap mSourceToTarget;
    std::shared_ptr<PDFDictionary> mWrittenPage;

    PDFRectangle DeterminePageBox(const std::shared_ptr<PDFDictionary> &inDictionary, EPDFPageBox inPageBoxType);
    charta::EStatusCode WritePageContentToSingleStream(IByteWriter *inTargetStream,
                                                       std::shared_ptr<PDFDictionary> inPageObject);
    charta::EStatusCode WritePDFStreamInputToStream(IByteWriter *inTargetStream,
                                                    const std::shared_ptr<PDFStreamInput> &inSourceStream);
    charta::EStatusCode CopyResourcesIndirectObjects(std::shared_ptr<PDFDictionary> inPage);
    void RegisterInDirectObjects(const std::shared_ptr<PDFDictionary> &inDictionary, ObjectIDTypeList &outNewObjects);
    void RegisterInDirectObjects(const std::shared_ptr<PDFArray> &inArray, ObjectIDTypeList &outNewObjects);
    charta::EStatusCode WriteNewObjects(const ObjectIDTypeList &inSourceObjectIDs);
    charta::EStatusCode WriteNewObjects(const ObjectIDTypeList &inSourceObjectIDs, ObjectIDTypeSet &ioCopiedObjects);
    charta::EStatusCode CopyInDirectObject(ObjectIDType inSourceObjectID, ObjectIDType inTargetObjectID,
                                           ObjectIDTypeSet &ioCopiedObjects);
    EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(
        const std::string &inPDFFilePath, const PDFParsingOptions &inParsingOptions, const PDFPageRange &inPageRange,
        IPageEmbedInFormCommand *inPageEmbedCommand, const double *inTransformationMatrix,
        const ObjectIDTypeList &inCopyAdditionalObjects, const ObjectIDTypeList &inPredefinedFormIDs);
    PDFFormXObject *CreatePDFFormXObjectForPage(const std::shared_ptr<PDFDictionary> &inPageObject,
                                                const PDFRectangle &inCropBox, const double *inTransformationMatrix,
                                                ObjectIDType inPredefinedObjectId);
    charta::EStatusCode CopyInDirectObject(ObjectIDType inSourceObjectID, ObjectIDType inTargetObjectID);

    charta::EStatusCode WriteObjectByType(const std::shared_ptr<PDFObject> &inObject, ETokenSeparator inSeparator,
                                          IObjectWritePolicy *inWritePolicy);
    charta::EStatusCode WriteArrayObject(const std::shared_ptr<PDFArray> &inArray, ETokenSeparator inSeparator,
                                         IObjectWritePolicy *inWritePolicy);
    charta::EStatusCode WriteDictionaryObject(const std::shared_ptr<PDFDictionary> &inDictionary,
                                              IObjectWritePolicy *inWritePolicy);
    charta::EStatusCode WriteStreamObject(const std::shared_ptr<PDFStreamInput> &inStream,
                                          IObjectWritePolicy *inWritePolicy);

    EStatusCodeAndObjectIDType CreatePDFPageForPage(unsigned long inPageIndex);

    charta::EStatusCode CopyPageContentToTargetPagePassthrough(PDFPage &inPage,
                                                               const std::shared_ptr<PDFDictionary> &inPageObject);
    charta::EStatusCode CopyPageContentToTargetPageRecoded(PDFPage &inPage,
                                                           std::shared_ptr<PDFDictionary> inPageObject);

    charta::EStatusCode WritePDFStreamInputToContentContext(PageContentContext *inContentContext,
                                                            const std::shared_ptr<PDFStreamInput> &inContentSource);
    charta::EStatusCode MergePDFPageForPage(PDFPage &inTargetPage, unsigned long inSourcePageIndex);
    charta::EStatusCode MergeResourcesToPage(PDFPage &inTargetPage, std::shared_ptr<PDFDictionary> inPage,
                                             StringToStringMap &outMappedResourcesNames);
    EStatusCodeAndObjectIDType CopyObjectToIndirectObject(const std::shared_ptr<PDFObject> &inObject);
    charta::EStatusCode CopyDirectObjectToIndirectObject(const std::shared_ptr<PDFObject> &inObject,
                                                         ObjectIDType inTargetObjectID);
    charta::EStatusCode MergePageContentToTargetPage(PDFPage &inTargetPage, std::shared_ptr<PDFDictionary> inSourcePage,
                                                     const StringToStringMap &inMappedResourcesNames);
    charta::EStatusCode WritePDFStreamInputToContentContext(PageContentContext *inContentContext,
                                                            const std::shared_ptr<PDFStreamInput> &inContentSource,
                                                            const StringToStringMap &inMappedResourcesNames);
    charta::EStatusCode WritePDFStreamInputToStream(IByteWriter *inTargetStream,
                                                    const std::shared_ptr<PDFStreamInput> &inSourceStream,
                                                    const StringToStringMap &inMappedResourcesNames);
    charta::EStatusCode ScanStreamForResourcesTokens(const std::shared_ptr<PDFStreamInput> &inSourceStream,
                                                     const StringToStringMap &inMappedResourcesNames,
                                                     ResourceTokenMarkerList &outResourceMarkers);
    charta::EStatusCode MergeAndReplaceResourcesTokens(IByteWriter *inTargetStream,
                                                       const std::shared_ptr<PDFStreamInput> &inSourceStream,
                                                       const StringToStringMap &inMappedResourcesNames,
                                                       const ResourceTokenMarkerList &inResourceMarkers);

    EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDFInContext(const PDFPageRange &inPageRange,
                                                                      IPageEmbedInFormCommand *inPageEmbedCommand,
                                                                      const double *inTransformationMatrix,
                                                                      const ObjectIDTypeList &inCopyAdditionalObjects,
                                                                      const ObjectIDTypeList &inPredefinedFormIDs);
    EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(IByteReaderWithPosition *inPDFStream,
                                                             const PDFParsingOptions &inParsingOptions,
                                                             const PDFPageRange &inPageRange,
                                                             IPageEmbedInFormCommand *inPageEmbedCommand,
                                                             const double *inTransformationMatrix,
                                                             const ObjectIDTypeList &inCopyAdditionalObjects,
                                                             const ObjectIDTypeList &inPredefinedFormIDs);
    EStatusCodeAndObjectIDTypeList AppendPDFPagesFromPDFInContext(const PDFPageRange &inPageRange,
                                                                  const ObjectIDTypeList &inCopyAdditionalObjects);
    charta::EStatusCode MergePDFPagesToPageInContext(PDFPage &inPage, const PDFPageRange &inPageRange,
                                                     const ObjectIDTypeList &inCopyAdditionalObjects);
    charta::EStatusCode StartCopyingContext(IByteReaderWithPosition *inPDFStream, const PDFParsingOptions &inOptions);
    charta::EStatusCode StartCopyingContext(PDFParser *inPDFParser);
    EStatusCode MergePDFPageForXObject(PDFFormXObject *inTargetFormXObject, unsigned long inSourcePageIndex);
    EStatusCode RegisterResourcesForForm(PDFFormXObject *inTargetFormXObject,
                                         const std::shared_ptr<PDFDictionary> &inPageObject,
                                         StringToStringMap &inMappedResourcesNames);

    std::string AsEncodedName(const std::string &inName);
    void RegisterResourcesForResourcesCategory(PDFFormXObject *inTargetFormXObject, ICategoryServicesCommand *inCommand,
                                               std::shared_ptr<PDFDictionary> inResourcesDictionary,
                                               ObjectIDTypeList &ioObjectsToLaterCopy,
                                               StringToStringMap &ioMappedResourcesNames);
    charta::EStatusCode MergePageContentToTargetXObject(PDFFormXObject *inTargetFormXObject,
                                                        std::shared_ptr<PDFDictionary> inSourcePage,
                                                        const StringToStringMap &inMappedResourcesNames);
    std::shared_ptr<PDFObject> FindPageResources(PDFParser *inParser,
                                                 const std::shared_ptr<PDFDictionary> &inDictionary);
};
