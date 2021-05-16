/*
 Source File : AppendingAndReading.cpp


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
#include "TestHelper.h"
#include "io/IByteReaderWithPosition.h"
#include "io/IByteWriterWithPosition.h"
#include "io/InputFile.h"
#include "io/OutputFile.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

static const uint8_t scTest[5] = {'T', 'e', 's', 't', '\n'};
static const uint8_t scTest1[6] = {'T', 'e', 's', 't', '1', '\n'};
static const uint8_t scTest2[6] = {'T', 'e', 's', 't', '2', '\n'};

TEST(Modification, AppendingAndReading)
{
    // checks if i can create a file, and then both read and write to it at the same
    // time with different classes. The test aims only for the scenario of reading from the part
    // that is not written in, as would be the case for PDFs with incremental updates

    EStatusCode status = eSuccess;

    // write initial file
    {
        OutputFile outputFile;

        status = outputFile.OpenFile(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "ReadAndWrite.txt"));
        ASSERT_EQ(status, PDFHummus::eSuccess);

        outputFile.GetOutputStream()->Write(scTest, 5);
        outputFile.GetOutputStream()->Write(scTest1, 6);
        outputFile.GetOutputStream()->Write(scTest2, 6);
    }

    // now open file for read and file for append

    InputFile inputFile;

    status = inputFile.OpenFile(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "ReadAndWrite.txt"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    OutputFile appendToFile;

    status = appendToFile.OpenFile(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "ReadAndWrite.txt"), true);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // check reading 1
    inputFile.GetInputStream()->SetPosition(5);
    uint8_t bufferTest1[6];

    inputFile.GetInputStream()->Read(bufferTest1, 6);
    ASSERT_EQ(memcmp(bufferTest1, scTest1, 6), 0);

    // check writing 1
    appendToFile.GetOutputStream()->Write(scTest1, 6);

    // check reading 2
    inputFile.GetInputStream()->Read(bufferTest1, 6);
    ASSERT_EQ(memcmp(bufferTest1, scTest2, 6), 0);

    // check reading 3
    inputFile.GetInputStream()->SetPosition(0);
    uint8_t bufferTest[5];
    inputFile.GetInputStream()->Read(bufferTest, 5);
    ASSERT_EQ(memcmp(bufferTest, scTest, 5), 0);

    // check writing 2
    appendToFile.GetOutputStream()->Write(scTest1, 6);
}