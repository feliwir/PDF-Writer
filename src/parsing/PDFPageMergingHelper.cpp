/*
 Source File : PDFPageMergingHelper.cpp


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

#include "parsing/PDFPageMergingHelper.h"
#include "PDFPage.h"
#include "PDFWriter.h"
#include "parsing/PDFDocumentCopyingContext.h"

using namespace charta;

PDFPageMergingHelper::PDFPageMergingHelper(PDFPage *inPage)
{
    mPage = inPage;
}

PDFPageMergingHelper::~PDFPageMergingHelper() = default;

EStatusCode PDFPageMergingHelper::MergePageContent(std::shared_ptr<charta::PDFDocumentCopyingContext> inCopyingContext,
                                                   unsigned long inPageIndex)
{
    return inCopyingContext->MergePDFPageToPage(*mPage, inPageIndex);
}

EStatusCode PDFPageMergingHelper::MergePageContent(PDFWriter *inWriter, const std::string &inPDFFilePath,
                                                   unsigned long inPageIndex)
{
    auto copyingContext = inWriter->CreatePDFCopyingContext(inPDFFilePath);

    if (copyingContext == nullptr)
    {
        return eFailure;
    }

    return MergePageContent(copyingContext, inPageIndex);
}

EStatusCode PDFPageMergingHelper::MergePageContent(PDFWriter *inWriter, charta::IByteReaderWithPosition *inPDFStream,
                                                   unsigned long inPageIndex)
{
    auto copyingContext = inWriter->CreatePDFCopyingContext(inPDFStream);

    if (copyingContext == nullptr)
    {
        return eFailure;
    }

    return MergePageContent(copyingContext, inPageIndex);
}
