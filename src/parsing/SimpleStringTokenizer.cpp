/*
   Source File : SimpleStringTokenizer.cpp


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
#include "parsing/SimpleStringTokenizer.h"
#include "io/IByteReader.h"
#include "io/OutputStringBufferStream.h"

using namespace charta;

SimpleStringTokenizer::SimpleStringTokenizer()
{
    mStream = nullptr;
    ResetReadState();
}

void SimpleStringTokenizer::SetReadStream(charta::IByteReader *inSourceStream)
{
    mStream = inSourceStream;
    ResetReadState();
}

void SimpleStringTokenizer::ResetReadState()
{
    mHasTokenBuffer = false;
    mStreamPositionTracker = 0;
    mRecentTokenPosition = 0;
}

void SimpleStringTokenizer::ResetReadState(const SimpleStringTokenizer &inExternalTokenizer)
{
    mTokenBuffer = inExternalTokenizer.mTokenBuffer;
    mHasTokenBuffer = inExternalTokenizer.mHasTokenBuffer;
    mStreamPositionTracker = inExternalTokenizer.mStreamPositionTracker;
    mRecentTokenPosition = inExternalTokenizer.mRecentTokenPosition;
}

// static const uint8_t scBackSlash[] = {'\\'};
static const std::string scStream = "stream";
// static const char scCR = '\r';
// static const char scLF = '\n';
BoolAndString SimpleStringTokenizer::GetNextToken()
{
    BoolAndString result;
    uint8_t buffer;
    OutputStringBufferStream tokenBuffer;

    if ((mStream == nullptr) || (!mStream->NotEnded() && !mHasTokenBuffer))
    {
        result.first = false;
        return result;
    }

    do
    {
        SkipTillToken();
        if (!mStream->NotEnded())
        {
            result.first = false;
            break;
        }

        // before reading the first byte save the token position, for external queries
        mRecentTokenPosition = mStreamPositionTracker;

        // get the first byte of the token
        if (GetNextByteForToken(buffer) != charta::eSuccess)
        {
            result.first = false;
            break;
        }
        tokenBuffer.Write(&buffer, 1);

        result.first = true; // will only be changed to false in case of read error

        while (mStream->NotEnded())
        {
            if (GetNextByteForToken(buffer) != charta::eSuccess)
            {
                result.first = false;
                break;
            }
            if (IsPDFWhiteSpace(buffer))
            {
                break;
            }
            if (IsPDFEntityBreaker(buffer))
            {
                SaveTokenBuffer(buffer); // for a non-space breaker, save the token for next token read
                break;
            }
            tokenBuffer.Write(&buffer, 1);
        }
        result.second = tokenBuffer.ToString();
    } while (false);

    return result;
}

void SimpleStringTokenizer::SkipTillToken()
{
    uint8_t buffer = 0;

    if (mStream == nullptr)
        return;

    // skip till hitting first non space, or segment end
    while (mStream->NotEnded())
    {
        if (GetNextByteForToken(buffer) != charta::eSuccess)
            break;

        if (!IsPDFWhiteSpace(buffer))
        {
            SaveTokenBuffer(buffer);
            break;
        }
    }
}

EStatusCode SimpleStringTokenizer::GetNextByteForToken(uint8_t &outByte)
{
    ++mStreamPositionTracker; // advance position tracker, because we are reading the next byte.
    if (mHasTokenBuffer)
    {
        outByte = mTokenBuffer;
        mHasTokenBuffer = false;
        return charta::eSuccess;
    }
    return (mStream->Read(&outByte, 1) != 1) ? charta::eFailure : charta::eSuccess;
}

static const uint8_t scWhiteSpaces[] = {0, 0x9, 0xA, 0xC, 0xD, 0x20};
bool SimpleStringTokenizer::IsPDFWhiteSpace(uint8_t inCharacter)
{
    bool isWhiteSpace = false;
    for (int i = 0; i < 6 && !isWhiteSpace; ++i)
        isWhiteSpace = (scWhiteSpaces[i] == inCharacter);
    return isWhiteSpace;
}

void SimpleStringTokenizer::SaveTokenBuffer(uint8_t inToSave)
{
    mHasTokenBuffer = true;
    mTokenBuffer = inToSave;
    --mStreamPositionTracker; // decreasing position trakcer, because it is as if the byte is put back in the stream
}

long long SimpleStringTokenizer::GetReadBufferSize() const
{
    return mHasTokenBuffer ? 1 : 0;
}

static const uint8_t scEntityBreakers[] = {'(', ')', '<', '>', ']', '[', '{', '}', '/', '%'};
bool SimpleStringTokenizer::IsPDFEntityBreaker(uint8_t inCharacter)
{
    bool isEntityBreak = false;
    for (int i = 0; i < 10 && !isEntityBreak; ++i)
        isEntityBreak = (scEntityBreakers[i] == inCharacter);
    return isEntityBreak;
}

long long SimpleStringTokenizer::GetRecentTokenPosition() const
{
    return mRecentTokenPosition;
}