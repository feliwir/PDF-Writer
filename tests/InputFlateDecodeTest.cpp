/*
   Source File : InputFlateDecodeTester.cpp


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
#include "TestHelper.h"
#include "io/InputFile.h"
#include "io/InputFlateDecodeStream.h"
#include "io/OutputFile.h"
#include "io/OutputFlateEncodeStream.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

TEST(PDFEmbedding, InputFlateDecode)
{
    OutputFile outputFile;
    std::string aString("hello world");
    uint8_t buffer;
    InputFlateDecodeStream inputDecoder;
    OutputFlateEncodeStream outputEncoder;

    outputFile.OpenFile(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "source.txt"));
    outputEncoder.Assign(outputFile.GetOutputStream());
    outputEncoder.Write((uint8_t *)aString.c_str(), aString.size());
    outputEncoder.Assign(nullptr);
    outputFile.CloseFile();

    InputFile inputFile;
    inputFile.OpenFile(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "source.txt"));
    inputDecoder.Assign(inputFile.GetInputStream());

    bool isSame = true;
    int i = 0;

    for (auto it = aString.begin(); it != aString.end() && isSame; ++it, ++i)
    {
        size_t amountRead = inputDecoder.Read(&buffer, 1);

        if (amountRead != 1)
        {
            isSame = false;
            break;
        }
        isSame = (*it) == buffer;
    }

    inputDecoder.Assign(nullptr);
    inputFile.CloseFile();

    ASSERT_TRUE(isSame);
}