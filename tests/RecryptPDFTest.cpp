/*
   Source File : TIFFImageTest.cpp


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

#include "PDFWriter.h"
#include "TestHelper.h"

#include <gtest/gtest.h>
#include <iostream>

TEST(Xcryption, RecryptPDF)
{
    EStatusCode status;

    // recrypt a document with AES encryption, and remove it
    status = PDFWriter::RecryptPDF(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/china.pdf"), "",
                                   RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "chinaWithoutEncryption.pdf"),
                                   LogConfiguration::DefaultLogConfiguration(), PDFCreationSettings(true, true));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // recrypt an encrypted document with no password
    status = PDFWriter::RecryptPDF(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/PDFWithPassword.pdf"), "user",
                                   RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "RecryptPDFWithPasswordToNothing.pdf"),
                                   LogConfiguration::DefaultLogConfiguration(), PDFCreationSettings(true, true));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // recrypt an encrypted document with new password
    status =
        PDFWriter::RecryptPDF(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/PDFWithPassword.pdf"), "user",
                              RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "RecryptPDFWithPasswordToNewPassword.pdf"),
                              LogConfiguration::DefaultLogConfiguration(),
                              PDFCreationSettings(true, true, EncryptionOptions("user1", 4, "owner1")));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // recrypt a plain to document to one with password
    status =
        PDFWriter::RecryptPDF(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/Original.pdf"), "",
                              RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "RecryptPDFOriginalToPasswordProtected.pdf"),
                              LogConfiguration::DefaultLogConfiguration(),
                              PDFCreationSettings(true, true, EncryptionOptions("user1", 4, "owner1")));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // same, but forcing AES
    status = PDFWriter::RecryptPDF(
        RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/Original.pdf"), "",
        RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "RecryptPDFOriginalToPasswordProtectedAES.pdf"),
        LogConfiguration::DefaultLogConfiguration(),
        PDFCreationSettings(true, true, EncryptionOptions("user1", 4, "owner1")), ePDFVersion16);
    ASSERT_EQ(status, PDFHummus::eSuccess);
}