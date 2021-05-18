/*
   Source File : UppercaseSequence.cpp


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
#include "UppercaseSequence.h"

#include <utility>

#include "Trace.h"

UppercaseSequence::UppercaseSequence()
{
    mSequanceString = "AAAAAA";
}

UppercaseSequence::UppercaseSequence(std::string inSequanceString)
{
    mSequanceString = std::move(inSequanceString);
}

void UppercaseSequence::Reset()
{
    mSequanceString = "AAAAAA";
}

UppercaseSequence::~UppercaseSequence()
{
}

const std::string &UppercaseSequence::GetNextValue()
{
    int i = 5;

    while (true)
    {
        if (mSequanceString.at(i) < 'Z')
        {
            ++mSequanceString.at(i);
            break;
        }

        if (i == 0)
        {
            TRACE_LOG("UppercaseSequence::GetNextValue, oops, reached overflow");
            break;
        }
        mSequanceString.at(i) = 'A';
        --i;
    }
    return mSequanceString;
}

const std::string &UppercaseSequence::ToString()
{
    return mSequanceString;
}

void UppercaseSequence::SetSequanceString(const std::string &inSequanceString)
{
    mSequanceString = inSequanceString;
}
