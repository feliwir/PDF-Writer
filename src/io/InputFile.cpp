/*
   Source File : InputFile.cpp


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
#include "io/InputFile.h"
#include "Trace.h"
#include "io/InputBufferedStream.h"
#include "io/InputFileStream.h"

charta::InputFile::InputFile() = default;

charta::InputFile::~InputFile()
{
    CloseFile();
}

charta::EStatusCode charta::InputFile::OpenFile(const std::string &inFilePath)
{

    EStatusCode status = CloseFile();
    if (status != charta::eSuccess)
    {
        TRACE_LOG1("charta::InputFile::OpenFile, Unexpected Failure. Couldn't close previously open file - %s",
                   mFilePath.c_str());
        return status;
    }

    auto inputFileStream = std::make_unique<InputFileStream>();
    status = inputFileStream->Open(inFilePath); // explicitly open, so status may be retrieved
    if (status != charta::eSuccess)
    {
        TRACE_LOG1("charta::InputFile::OpenFile, Unexpected Failure. Cannot open file for reading - %s",
                   inFilePath.c_str());
        return status;
    }

    mInputStream = std::make_unique<InputBufferedStream>(std::move(inputFileStream));
    mFilePath = inFilePath;

    return status;
}

charta::EStatusCode charta::InputFile::CloseFile()
{
    if (nullptr == mInputStream)
    {
        return charta::eSuccess;
    }

    auto *inputFileStream = (InputFileStream *)mInputStream->GetSourceStream();
    EStatusCode status = inputFileStream->Close(); // explicitly close, so status may be retrieved

    mInputStream = nullptr;
    return status;
}

charta::IByteReaderWithPosition *charta::InputFile::GetInputStream()
{
    return mInputStream.get();
}

const std::string &charta::InputFile::GetFilePath()
{
    return mFilePath;
}

long long charta::InputFile::GetFileSize()
{
    if (mInputStream != nullptr)
    {
        auto *inputFileStream = (InputFileStream *)mInputStream->GetSourceStream();

        return inputFileStream->GetFileSize();
    }
    return 0;
}