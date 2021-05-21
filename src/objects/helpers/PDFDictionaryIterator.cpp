///////////////////////////////////////////////////////////////////////////////
// PDFDictionaryIterator.cpp Stefan Woerthmueller 2014  StefanWoe@googlemail.com
///////////////////////////////////////////////////////////////////////////////

#include "objects/helpers/PDFDictionaryIterator.h"
#include "objects/helpers/PDFArrayIterator.h"

#include <fstream>
#include <iostream>
#include <ostream>

PDFArrayIterator PDFDictionaryIterator::Array(const std::string &s)
{
    if (mDictonary == nullptr)
        return PDFArrayIterator(mParser);

    PDFObjectCastPtr<PDFArray> foundArray(mDictonary->QueryDirectObject(s));
    if (foundArray.GetPtr() != nullptr)
        return PDFArrayIterator(mParser, foundArray);
    return PDFArrayIterator(mParser);
}

PDFDictionaryIterator PDFDictionaryIterator::Enter(const std::string &s)
{
    if (mDictonary == nullptr)
        return PDFDictionaryIterator(mParser);
    PDFObjectCastPtr<PDFIndirectObjectReference> foundReference(mDictonary->QueryDirectObject(s));
    if (!foundReference)
        return PDFDictionaryIterator(mParser);

    PDFObjectCastPtr<PDFDictionary> catalog(mParser.ParseNewObject(foundReference->mObjectID));
    if (!catalog)
        return PDFDictionaryIterator(mParser);

    return PDFDictionaryIterator(mParser, catalog);
}

std::string PDFDictionaryIterator::GetStrValue(const std::string &s)
{
    if (mDictonary == nullptr)
        return std::string();
    PDFObjectCastPtr<PDFLiteralString> foundReference(mDictonary->QueryDirectObject(s));
    if (!foundReference)
        return std::string();
    return foundReference->GetValue();
}

bool PDFDictionaryIterator::WriteStreamToFile(InputFile &pdfFile, const std::string &s, const std::string &filePath)
{
    if (mDictonary == nullptr)
        return false;
    PDFObjectCastPtr<PDFIndirectObjectReference> foundReference(mDictonary->QueryDirectObject(s));
    if (!foundReference)
        return false;

    PDFObjectCastPtr<PDFStreamInput> foundStreamInput(mParser.ParseNewObject(foundReference->mObjectID));
    if (!foundStreamInput)
        return false;

    std::ofstream myFile;
    myFile.open(filePath.c_str(), std::ios_base::trunc | std::ios_base::binary);
    if (!myFile.is_open())
        return false;

    IByteReader *streamReader = mParser.CreateInputStreamReader(foundStreamInput);
    if (streamReader == nullptr)
        return false;

    uint8_t buffer[0xffff];
    if (streamReader != nullptr)
    {
        pdfFile.GetInputStream()->SetPosition(foundStreamInput->GetStreamContentStart());
        while (streamReader->NotEnded())
        {
            size_t readAmount = streamReader->Read(buffer, sizeof(buffer));
            myFile.write((const char *)buffer, readAmount);
        }
    }
    delete streamReader;

    return true;
}
