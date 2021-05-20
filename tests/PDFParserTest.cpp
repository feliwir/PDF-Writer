/*
   Source File : PDFParserTest.cpp


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
#include "parsing/PDFParser.h"
#include "ObjectsBasicTypes.h"
#include "PrimitiveObjectsWriter.h"
#include "TestHelper.h"
#include "io/IByteWriterWithPosition.h"
#include "io/InputFile.h"
#include "io/OutputFile.h"
#include "objects/PDFArray.h"
#include "objects/PDFDictionary.h"
#include "objects/PDFIndirectObjectReference.h"
#include "objects/PDFObject.h"
#include "objects/PDFObjectCast.h"
#include "objects/PDFStreamInput.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

static const char *scIndirectStart = "Indirect object reference:\r\n";
static const char *scParsedAlready = "was parsed already\r\n";
static const char *scIteratingStreamDict = "Stream . iterating stream dictionary:\r\n";

static int mTabLevel = 0;
static std::set<ObjectIDType> mIteratedObjectIDs;

EStatusCode IterateObjectTypes(PDFObject *inObject, PDFParser &inParser, IByteWriter *inOutput)
{
    PrimitiveObjectsWriter primitivesWriter;

    primitivesWriter.SetStreamForWriting(inOutput);

    for (int i = 0; i < mTabLevel; ++i)
        inOutput->Write((const uint8_t *)"  ", 2);

    if (inObject->GetType() == PDFObject::ePDFObjectIndirectObjectReference)
    {
        inOutput->Write((const uint8_t *)scIndirectStart, strlen(scIndirectStart));
        if (mIteratedObjectIDs.find(((PDFIndirectObjectReference *)inObject)->mObjectID) == mIteratedObjectIDs.end())
        {
            mIteratedObjectIDs.insert(((PDFIndirectObjectReference *)inObject)->mObjectID);
            auto pointedObject(
                inParser.ParseNewObject(((PDFIndirectObjectReference *)inObject)->mObjectID));
            if (!pointedObject)
            {
                std::cout << "\nFailed to retreive object of ID ="
                          << ((PDFIndirectObjectReference *)inObject)->mObjectID << "\n";
                return PDFHummus::eFailure;
            }
            return IterateObjectTypes(pointedObject.GetPtr(), inParser, inOutput);
        }

        for (int i = 0; i < mTabLevel; ++i)
            inOutput->Write((const uint8_t *)"  ", 2);
        inOutput->Write((const uint8_t *)scParsedAlready, strlen(scParsedAlready));
        return PDFHummus::eSuccess;
    }
    else if (inObject->GetType() == PDFObject::ePDFObjectArray)
    {
        primitivesWriter.WriteKeyword(PDFObject::scPDFObjectTypeLabel(inObject->GetType()));
        ++mTabLevel;
        PDFObjectCastPtr<PDFArray> anArray;
        anArray = inObject; // do assignment here, otherwise it's considered constructor...which won't addref
        SingleValueContainerIterator<PDFObjectVector> it = anArray->GetIterator();
        EStatusCode status = PDFHummus::eSuccess;
        while (it.MoveNext() && PDFHummus::eSuccess == status)
            status = IterateObjectTypes(it.GetItem(), inParser, inOutput);
        --mTabLevel;
        return status;
    }
    else if (inObject->GetType() == PDFObject::ePDFObjectDictionary)
    {
        primitivesWriter.WriteKeyword(PDFObject::scPDFObjectTypeLabel(inObject->GetType()));
        ++mTabLevel;
        PDFObjectCastPtr<PDFDictionary> aDictionary;
        aDictionary = inObject; // do assignment here, otherwise it's considered constructor...which won't addref
        MapIterator<PDFNameToPDFObjectMap> it = aDictionary->GetIterator();

        EStatusCode status = PDFHummus::eSuccess;
        while (it.MoveNext() && PDFHummus::eSuccess == status)
        {
            status = IterateObjectTypes(it.GetKey(), inParser, inOutput);
            if (PDFHummus::eSuccess == status)
                status = IterateObjectTypes(it.GetValue(), inParser, inOutput);
        }
        --mTabLevel;
        return status;
    }
    else if (inObject->GetType() == PDFObject::ePDFObjectStream)
    {
        inOutput->Write((const uint8_t *)scIteratingStreamDict, strlen(scIteratingStreamDict));
        PDFObjectCastPtr<PDFDictionary> aDictionary(((PDFStreamInput *)inObject)->QueryStreamDictionary());
        return IterateObjectTypes(aDictionary.GetPtr(), inParser, inOutput);
    }
    else
    {
        primitivesWriter.WriteKeyword(PDFObject::scPDFObjectTypeLabel(inObject->GetType()));
        return PDFHummus::eSuccess;
    }
}

TEST(PDFEmbedding, PDFParser)
{
    EStatusCode status = PDFHummus::eSuccess;
    InputFile pdfFile;
    PDFParser parser;
    OutputFile outputFile;

    status = pdfFile.OpenFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/XObjectContent.pdf"));
    ASSERT_EQ(status, eSuccess);

    status = parser.StartPDFParsing(pdfFile.GetInputStream());
    ASSERT_EQ(status, eSuccess);

    // now let's do something with what got parsed

    ASSERT_EQ(parser.GetPDFLevel(), 1.3);
    ASSERT_EQ(parser.GetPagesCount(), 2);

    // now just iterate, and print the object types
    PDFObjectCastPtr<PDFDictionary> catalog(parser.QueryDictionaryObject(parser.GetTrailer(), "Root"));
    ASSERT_NE(catalog.GetPtr(), nullptr);

    mTabLevel = 0;
    status = outputFile.OpenFile(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "PDFParserTestOutput.txt"));

    status = IterateObjectTypes(catalog.GetPtr(), parser, outputFile.GetOutputStream());
    ASSERT_EQ(status, eSuccess);
}