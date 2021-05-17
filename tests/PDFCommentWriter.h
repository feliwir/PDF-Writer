#pragma once

#include "EStatusCode.h"
#include "ObjectsBasicTypes.h"
#include "PDFComment.h"
#include <map>
#include <utility>

class PDFWriter;

class PDFCommentWriter
{
  public:
    PDFCommentWriter(PDFWriter *inPDFWriter);

    // Write a comment (now) and attach it to the page that will be written next.
    // Technically speaking, the comment will be written at this point (so don't have an open content stream, or
    // something), as well as any comments that this one is connected to through "In Reply To" relationship.
    PDFHummus::EStatusCode AttachCommentTreeToNextPage(PDFComment *inComment,
                                                       std::map<PDFComment *, ObjectIDType> &ioPageCommentsContext);

    // Write a comment (now), and its connected comments, and return the comment object ID. This workflow
    // is for when you don't wish to use the internal infrastracture to write to the next page.
    // Then - you have to do the page attachment on your own. returns a map
    // of comments to ids. note that since you may have replytos, this may be more than one comments
    PDFHummus::EStatusCode WriteCommentTree(PDFComment *inComment,
                                            std::map<PDFComment *, ObjectIDType> &ioPageCommentsContext);

  private:
    PDFWriter *mPDFWriter;

    void ListenOnPageWrites();
    std::pair<PDFHummus::EStatusCode, ObjectIDType> WriteCommentsTree(
        PDFComment *inComment, std::map<PDFComment *, ObjectIDType> &inCommentsContext);
};
