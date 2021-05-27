/*
   Source File : PDFTextStringTest.cpp


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
#include "PDFTextString.h"
#include "TestHelper.h"
#include "encoding/UnicodeString.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace charta;

TEST(ObjectContext, PDFTextString)
{
    EStatusCode status = charta::eSuccess;

    // Empty String test
    PDFTextString emptyString;
    ASSERT_TRUE(emptyString.ToString().empty());

    // PDFEncoded test, Latin
    PDFTextString latinString("Hello World");
    ASSERT_EQ(latinString.ToString(), "Hello World");

    // PDFEncoded test, special char
    UnicodeString aString;
    aString.GetUnicodeList().push_back(0x20AC);

    PDFTextString latinSpecialString;
    latinSpecialString.FromUTF8(aString.ToUTF8().second);
    ASSERT_EQ(latinSpecialString.ToString(), "\xA0");

    // UTF16 test
    UnicodeString bString;
    bString.GetUnicodeList().push_back(0x20AB);

    PDFTextString latinUTF16String;
    latinUTF16String.FromUTF8(bString.ToUTF8().second);
    ASSERT_EQ(latinUTF16String.ToString(), "\xFE\xFF\x20\xAB");
}