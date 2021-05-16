/*
   Source File : OutputFile.cpp


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
#include "io/OutputFile.h"
#include "Trace.h"
#include "io/OutputBufferedStream.h"
#include "io/OutputFileStream.h"

using namespace PDFHummus;

OutputFile::OutputFile()
{
    mOutputStream = nullptr;
    mFileStream = nullptr;
}

OutputFile::~OutputFile()
{
    CloseFile();
}

EStatusCode OutputFile::OpenFile(const std::string &inFilePath, bool inAppend)
{
    EStatusCode status;
    do
    {
        status = CloseFile();
        if (status != PDFHummus::eSuccess)
        {
            TRACE_LOG1("OutputFile::OpenFile, Unexpected Failure. Couldn't close previously open file - %s",
                       mFilePath.c_str());
            break;
        }

        auto *outputFileStream = new OutputFileStream();
        status = outputFileStream->Open(inFilePath, inAppend); // explicitly open, so status may be retrieved
        if (status != PDFHummus::eSuccess)
        {
            TRACE_LOG1("OutputFile::OpenFile, Unexpected Failure. Cannot open file for writing - %s",
                       inFilePath.c_str());
            delete outputFileStream;
            break;
        }

        mOutputStream = new OutputBufferedStream(outputFileStream);
        mFileStream = outputFileStream;
        mFilePath = inFilePath;
    } while (false);
    return status;
}

EStatusCode OutputFile::CloseFile()
{
    if (nullptr == mOutputStream)
    {
        return PDFHummus::eSuccess;
    }
    else
    {
        mOutputStream->Flush();
        EStatusCode status = mFileStream->Close(); // explicitly close, so status may be retrieved

        delete mOutputStream; // will delete the referenced file stream as well
        mOutputStream = nullptr;
        mFileStream = nullptr;
        return status;
    }
}

IByteWriterWithPosition *OutputFile::GetOutputStream()
{
    return mOutputStream;
}

const std::string &OutputFile::GetFilePath()
{
    return mFilePath;
}
