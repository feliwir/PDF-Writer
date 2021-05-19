///////////////////////////////////////////////////////////////////////////////
// PDFDictionaryIterator.h  Stefan Woerthmueller 2014  StefanWoe@googlemail.com
///////////////////////////////////////////////////////////////////////////////
/*
    Usage Example:

    PDFParser parser;
    InputFile pdfFile;
    if(pdfFile.OpenFile(Path) != PDFHummus::eSuccess)
        return;
    parser.StartPDFParsing(pdfFile.GetInputStream());

    PDFDictionaryIterator it(parser, parser.GetTrailer());
    PDFArrayIterator it2 =
   it.Enter("Root").Enter("Names").Enter("EmbeddedFiles").Array("Kids").DictAt(0).Array("Names");

    for(int i = 1; i < it2.GetLength(); i += 2)
    {
        PDFDictionaryIterator dict = it2.DictAt(i);
        std::string fileName = dict.GetStrValue("F");
        std::string outPath;
        if(dict.Enter("EF").WriteStreamToFile(pdfFile, "F", outPath))
        {
            ... Handle Error
        }
    }

*/

#pragma once
#include "PDFWriter.h"
#include "objects/PDFIndirectObjectReference.h"
#include "objects/PDFLiteralString.h"
#include "objects/PDFObjectCast.h"
#include "objects/PDFStreamInput.h"

class PDFArrayIterator;
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class PDFDictionaryIterator
{
  public:
    PDFDictionaryIterator(PDFParser &parser) : mDictonary(NULL), mParser(parser)
    {
    }

    PDFDictionaryIterator(PDFParser &parser, PDFObjectCastPtr<PDFDictionary> &inDict)
        : mDictonary(inDict.GetPtr()), mDictonaryRefPtr(inDict), mParser(parser)
    {
    }
    PDFDictionaryIterator(PDFParser &parser, std::shared_ptr<PDFDictionary> inDict)
        : mDictonary(inDict), mParser(parser)
    {
    }
    std::string GetStrValue(const std::string &s);

    PDFDictionaryIterator Enter(const std::string &s);

    PDFArrayIterator Array(const std::string &s);

    bool WriteStreamToFile(InputFile &pdfFile, const std::string &s, const std::string &filePath);

  private:
    void SetDictRefPtr(std::shared_ptr<PDFDictionary> dictonary)
    {
        mDictonary = dictonary;
        mDictonaryRefPtr = dictonary;
    }
    std::shared_ptr<PDFDictionary> mDictonary;
    PDFObjectCastPtr<PDFDictionary> mDictonaryRefPtr;
    PDFParser &mParser;
};
