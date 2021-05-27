/*
   Source File : FlateEncryptionTest.cpp


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
#include "io/InputBufferedStream.h"
#include "io/InputFileStream.h"
#include "io/OutputBufferedStream.h"
#include "io/OutputFileStream.h"
#include "io/OutputFlateDecodeStream.h"
#include "io/OutputFlateEncodeStream.h"
#include "io/OutputStreamTraits.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace charta;

TEST(IO, FlateEncryption)
{
    EStatusCode status;

    // Create encrypted copy of the message
    std::string aString = "encryptedMessage";

    IByteWriter *encoderStream = new OutputFlateEncodeStream(new OutputBufferedStream(
        std::make_unique<OutputFileStream>(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "encrypted.txt"))));

    size_t writtenSize = encoderStream->Write((const uint8_t *)aString.c_str(), aString.length());
    delete encoderStream;
    ASSERT_EQ(writtenSize, aString.length());

    IByteReader *encoderReaderStream = new InputBufferedStream(
        std::make_unique<InputFileStream>(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "encrypted.txt")));
    IByteWriter *decoderWriterStream = new OutputFlateDecodeStream(new OutputBufferedStream(
        std::make_unique<OutputFileStream>(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "decrypted.txt"))));

    OutputStreamTraits outputTraits(decoderWriterStream);
    status = outputTraits.CopyToOutputStream(encoderReaderStream);
    ASSERT_EQ(status, charta::eSuccess);

    delete encoderReaderStream;
    delete decoderWriterStream;

    // now read again decrypted and compare to original message
    IByteReader *decoderReaderStream = new InputBufferedStream(
        std::make_unique<InputFileStream>(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "decrypted.txt")));
    char buffer[256];

    size_t readSize = decoderReaderStream->Read((uint8_t *)buffer, 255);
    buffer[readSize] = 0;

    delete decoderReaderStream;

    ASSERT_EQ(strcmp(aString.c_str(), buffer), 0);
}