///////////////////////////////////////////////////////////////////////////////
// PDFArrayIterator.cpp    Stefan Woerthmueller 2014  StefanWoe@googlemail.com
///////////////////////////////////////////////////////////////////////////////

#include "objects/helpers/PDFArrayIterator.h"
#include "objects/helpers/PDFDictionaryIterator.h"

PDFDictionaryIterator PDFArrayIterator::DictAt(int ndx)
{
    if (mArray.GetPtr() == nullptr)
        return PDFDictionaryIterator(mParser);
    PDFObjectCastPtr<charta::PDFIndirectObjectReference> foundReference(mArray->QueryObject(ndx));
    if (!foundReference)
        return PDFDictionaryIterator(mParser);

    PDFObjectCastPtr<charta::PDFDictionary> catalog(mParser.ParseNewObject(foundReference->mObjectID));
    if (!catalog)
        return PDFDictionaryIterator(mParser);

    return PDFDictionaryIterator(mParser, catalog);
}

unsigned long PDFArrayIterator::GetLength()
{
    if (mArray.GetPtr() == nullptr)
        return 0;
    return mArray->GetLength();
}
