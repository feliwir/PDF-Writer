///////////////////////////////////////////////////////////////////////////////
// PDFArrayIterator.h       Stefan Woerthmueller 2014  StefanWoe@googlemail.com
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "PDFWriter.h"
#include "objects/PDFArray.h"
#include "objects/PDFObjectCast.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class PDFDictionaryIterator;

class PDFArrayIterator
{
  public:
    PDFArrayIterator(PDFParser &parser) : mParser(parser)
    {
    }
    PDFArrayIterator(PDFParser &parser, PDFObjectCastPtr<charta::PDFArray> &inArray) : mArray(inArray), mParser(parser)
    {
    }

    PDFDictionaryIterator DictAt(int ndx);
    unsigned long GetLength();

  private:
    PDFObjectCastPtr<charta::PDFArray> mArray;
    PDFParser &mParser;
};
