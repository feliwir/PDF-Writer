/*
Source File : FlateObjectDecodeTest.cpp


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
#include "EStatusCode.h"
#include "TestHelper.h"
#include "io/IByteReader.h"
#include "io/InputFile.h"
#include "objects/PDFStreamInput.h"
#include "parsing/PDFParser.h"

#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <string>

using namespace charta;

#define BUF_SIZE 1024

EStatusCode decodeStream(const std::string &sourcePath, const std::string &targetPath)
{
    PDFParser parser;
    InputFile pdfFile;
    EStatusCode status = pdfFile.OpenFile(sourcePath);
    if (status != eSuccess)
        return status;

    status = parser.StartPDFParsing(pdfFile.GetInputStream());
    if (status != eSuccess)
        return status;

    // Parse image object
    auto streamObj = parser.ParseNewObject(7);
    if (streamObj == nullptr || streamObj->GetType() != PDFObject::ePDFObjectStream)
        return eFailure;

    auto stream = std::static_pointer_cast<PDFStreamInput>(streamObj);
    charta::IByteReader *reader = parser.StartReadingFromStream(stream);
    if (reader == nullptr)
        return eFailure;

    std::ofstream os(targetPath.c_str(), std::ofstream::binary);
    uint8_t buffer[1024];
    size_t total = 0;
    while (reader->NotEnded())
    {
        size_t readAmount = reader->Read(buffer, BUF_SIZE);
        os.write((char *)buffer, readAmount);
        total += readAmount;
    }

    delete reader;
    os.close();

    std::cout << "Total read: " << total << "\n";
    std::cout << "Expected read: " << 120000 << "\n";

    return total == 120000 ? eSuccess : eFailure;
}

TEST(Parsing, FlateObjectDecodeTest)
{
    ASSERT_EQ(decodeStream(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/test2.pdf"),
                           RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "image.data")),
              eSuccess);
}