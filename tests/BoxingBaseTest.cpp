/*
   Source File : BoxingBaseTest.cpp


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
#include "BoxingBase.h"
#include "TestHelper.h"
#include <gtest/gtest.h>

#include <iostream>

TEST(Patterns, BoxingBase)
{
    // Assignment
    Int a = 3;
    a = 2;
    EXPECT_EQ(a, 2);

    // Initialization
    Int b(2);
    EXPECT_EQ(b, 2);

    // From string
    Int c("2");
    EXPECT_EQ(c, 2);

    // From wide string
    Int d(L"2");
    EXPECT_EQ(d, 2);

    // string write
    Int e(2);
    EXPECT_EQ(e.ToString(), "2");

    // wide string write
    Int f(2);
    EXPECT_EQ(f.ToWString(), L"2");
}