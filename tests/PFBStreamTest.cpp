/*
   Source File : PFBStreamTest.cpp


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
#include "io/IByteWriterWithPosition.h"
#include "io/InputFile.h"
#include "io/InputPFBDecodeStream.h"
#include "io/OutputFile.h"
#include "io/OutputStreamTraits.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

TEST(Type1, PFBStream)
{
    EStatusCode status;
    InputFile pfbFile;
    OutputFile decodedPFBFile;
    InputPFBDecodeStream decodeStream;

    pfbFile.OpenFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/HLB_____.PFB"));

    decodedPFBFile.OpenFile(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "decodedPFBFile.txt"));

    status = decodeStream.Assign(pfbFile.GetInputStream());

    OutputStreamTraits traits(decodedPFBFile.GetOutputStream());

    status = traits.CopyToOutputStream(&decodeStream);

    decodeStream.Assign(nullptr);
    pfbFile.CloseFile();
    decodedPFBFile.CloseFile();
}