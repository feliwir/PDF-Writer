/*
   Source File : Ascii7Encoding.cpp


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
#include "encoding/Ascii7Encoding.h"
#include "io/OutputStringBufferStream.h"

charta::BoolAndString charta::Ascii7Encoding::Encode(const std::string &inString)
{
    OutputStringBufferStream asciiString;
    bool encodingGood = true;
    uint8_t buffer;
    std::string::const_iterator it = inString.begin();

    for (; it != inString.end() && encodingGood; ++it)
    {
        if (((uint8_t)*it) <= 127)
        {
            buffer = (char)*it;
            asciiString.Write(&buffer, 1);
        }
        else
            encodingGood = false;
    }

    return BoolAndString(encodingGood, asciiString.ToString());
}
