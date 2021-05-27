/*
   Source File : FreeTypeWrapper.cpp


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
#include "text/freetype/FreeTypeWrapper.h"
#include "Trace.h"
#include "io/IByteReaderWithPosition.h"
#include "io/InputFile.h"

using namespace charta;

#undef __FTERRORS_H__
#define FT_ERRORDEF(e, v, s) {e, s},
#define FT_ERROR_START_LIST {
#define FT_ERROR_END_LIST                                                                                              \
    {                                                                                                                  \
        0, 0                                                                                                           \
    }                                                                                                                  \
    }                                                                                                                  \
    ;

static const struct
{
    int err_code;
    const char *err_msg;
} ft_errors[] =

#include FT_ERRORS_H

    FreeTypeWrapper::FreeTypeWrapper()
{
    if (FT_Init_FreeType(&mFreeType) != 0)
    {
        TRACE_LOG("FreeTypeWrapper::FreeTypeWrapper, unexpected failure. failed to initialize Free Type");
        mFreeType = nullptr;
    }
}

FreeTypeWrapper::~FreeTypeWrapper()
{
    auto it = mOpenStreams.begin();
    for (; it != mOpenStreams.end(); ++it)
    {
        auto itStreams = it->second.begin();
        for (; itStreams != it->second.end(); ++itStreams)
        {
            delete *itStreams;
        }
    }
    mOpenStreams.clear();
    if (mFreeType != nullptr)
        FT_Done_FreeType(mFreeType);
}

// using my own streams, to implement UTF8 paths
FT_Face FreeTypeWrapper::NewFace(const std::string &inFilePath, FT_Long inFontIndex)
{
    FT_Face face;
    FT_Open_Args openFaceArguments;

    do
    {
        if (FillOpenFaceArgumentsForUTF8String(inFilePath, openFaceArguments) != charta::eSuccess)
        {
            face = nullptr;
            break;
        }

        FT_Error ftStatus = FT_Open_Face(mFreeType, &openFaceArguments, inFontIndex, &face);

        if (ftStatus != 0)
        {
            TRACE_LOG2("FreeTypeWrapper::NewFace, unable to load font named %s with index %ld", inFilePath.c_str(),
                       inFontIndex);
            TRACE_LOG2("FreeTypeWrapper::NewFace, Free Type Error, Code = %d, Message = %s",
                       ft_errors[ftStatus].err_code, ft_errors[ftStatus].err_msg);
            face = nullptr;
        }

    } while (false);

    if (face == nullptr)
        CloseOpenFaceArgumentsStream(openFaceArguments);
    else
        RegisterStreamForFace(face, openFaceArguments.stream);
    return face;
}

EStatusCode FreeTypeWrapper::FillOpenFaceArgumentsForUTF8String(const std::string &inFilePath, FT_Open_Args &ioArgs)
{
    ioArgs.flags = FT_OPEN_STREAM;
    ioArgs.memory_base = nullptr;
    ioArgs.memory_size = 0;
    ioArgs.pathname = nullptr;
    ioArgs.driver = nullptr;
    ioArgs.num_params = 0;
    ioArgs.params = nullptr;
    ioArgs.stream = CreateFTStreamForPath(inFilePath);

    if (ioArgs.stream != nullptr)
    {
        return charta::eSuccess;
    }

    TRACE_LOG1("FreeTypeWrapper::FillOpenFaceArgumentsForWideString, Cannot Open file for reading %s",
               inFilePath.c_str());
    return charta::eFailure;
}

void FreeTypeWrapper::CloseOpenFaceArgumentsStream(FT_Open_Args &ioArgs)
{
    if (ioArgs.stream != nullptr)
    {
        delete (InputFile *)(ioArgs.stream->descriptor.pointer);
        delete ioArgs.stream;
    }
}

void FreeTypeWrapper::RegisterStreamForFace(FT_Face inFace, FT_Stream inStream)
{
    auto it = mOpenStreams.find(inFace);
    if (it == mOpenStreams.end())
        it = mOpenStreams.insert(FTFaceToFTStreamListMap::value_type(inFace, FTStreamList())).first;
    it->second.push_back(inStream);
}

FT_Face FreeTypeWrapper::NewFace(const std::string &inFilePath, const std::string &inSecondaryFilePath,
                                 FT_Long inFontIndex)
{
    FT_Open_Args attachStreamArguments;

    FT_Face face = NewFace(inFilePath, inFontIndex);
    if (face != nullptr)
    {
        do
        {
            if (FillOpenFaceArgumentsForUTF8String(inSecondaryFilePath, attachStreamArguments) != charta::eSuccess)
            {
                DoneFace(face);
                face = nullptr;
                break;
            }

            FT_Error ftStatus = FT_Attach_Stream(face, &attachStreamArguments);
            if (ftStatus != 0)
            {
                TRACE_LOG1("FreeTypeWrapper::NewFace, unable to load secondary file %s", inSecondaryFilePath.c_str());
                TRACE_LOG2("FreeTypeWrapper::NewFace, Free Type Error, Code = %d, Message = %s",
                           ft_errors[ftStatus].err_code, ft_errors[ftStatus].err_msg);
                DoneFace(face);
                face = nullptr;
            }
        } while (false);

        if (face == nullptr)
            CloseOpenFaceArgumentsStream(attachStreamArguments);
        else
            RegisterStreamForFace(face, attachStreamArguments.stream);
    }

    return face;
}

FT_Error FreeTypeWrapper::DoneFace(FT_Face ioFace)
{
    FT_Error status = FT_Done_Face(ioFace);
    CleanStreamsForFace(ioFace);
    return status;
}

void FreeTypeWrapper::CleanStreamsForFace(FT_Face inFace)
{
    auto it = mOpenStreams.find(inFace);
    if (it != mOpenStreams.end())
    {
        auto itStreams = it->second.begin();
        for (; itStreams != it->second.end(); ++itStreams)
        {
            delete *itStreams;
        }
    }
    mOpenStreams.erase(it);
}

FT_Library FreeTypeWrapper::operator->()
{
    return mFreeType;
}

static unsigned long InputFileReadSeek(FT_Stream stream, unsigned long offset, unsigned char *buffer,
                                       unsigned long count)
{
    IByteReaderWithPosition *inputFileStream = ((InputFile *)(stream->descriptor.pointer))->GetInputStream();
    unsigned long readBytes = 0;

    inputFileStream->SetPosition(offset);
    if (count > 0)
        readBytes = (unsigned long)inputFileStream->Read(buffer, count);
    return readBytes;
}

static void InputFileClose(FT_Stream stream)
{
    delete (InputFile *)(stream->descriptor.pointer);
    stream->descriptor.pointer = nullptr;
}

FT_Stream FreeTypeWrapper::CreateFTStreamForPath(const std::string &inFilePath)
{
    auto *inputFile = new InputFile;

    if (inputFile->OpenFile(inFilePath) != charta::eSuccess)
        return nullptr;

    auto *aStream = new FT_StreamRec();

    aStream->base = nullptr;
    aStream->size = (unsigned long)inputFile->GetFileSize();
    aStream->pos = 0;
    aStream->descriptor.pointer = inputFile;
    aStream->pathname.pointer = nullptr;
    aStream->read = InputFileReadSeek;
    aStream->close = InputFileClose;
    aStream->memory = nullptr;
    aStream->cursor = nullptr;
    aStream->limit = nullptr;

    return aStream;
}

FreeTypeWrapper::operator FT_Library() const
{
    return mFreeType;
}
