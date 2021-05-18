/*
   Source File : PDFDateTest.cpp


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
#include "PDFDate.h"
#include <ctime>
#include <stdio.h>

#include <gtest/gtest.h>
#include <iostream>

TEST(ObjectContext, PDFDate)
{
    // Empty Date
    PDFDate emptyDate;
    ASSERT_TRUE(emptyDate.ToString().empty());

    // Year only Date
    PDFDate yearDate;
    yearDate.Year = 1984;
    ASSERT_EQ(yearDate.ToString(), "D:1984");

    // Day only Date
    PDFDate dayDate;
    dayDate.Year = 1984;
    dayDate.Month = 4;
    dayDate.Day = 3;
    ASSERT_EQ(dayDate.ToString(), "D:19840403");

    // local Time Date
    PDFDate localTimeDate;
    localTimeDate.Year = 1984;
    localTimeDate.Month = 4;
    localTimeDate.Day = 3;
    localTimeDate.Hour = 18;
    localTimeDate.Minute = 30;
    localTimeDate.Second = 45;
    ASSERT_EQ(localTimeDate.ToString(), "D:19840403183045");

    // Fully qualified Time date
    PDFDate fullDate;
    fullDate.Year = 1984;
    fullDate.Month = 4;
    fullDate.Day = 3;
    fullDate.Hour = 18;
    fullDate.Minute = 30;
    fullDate.Second = 45;
    fullDate.UTC = PDFDate::eEarlier;
    fullDate.HourFromUTC = 2;
    fullDate.MinuteFromUTC = 0;
    ASSERT_EQ(fullDate.ToString(), "D:19840403183045-02'00'");
}