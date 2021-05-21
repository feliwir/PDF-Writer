/*
   Source File : InputPFBDecodeStream.cpp


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
#include "io/InputPFBDecodeStream.h"
#include "Trace.h"
#include "io/OutputStringBufferStream.h"

using namespace PDFHummus;

InputPFBDecodeStream::InputPFBDecodeStream()
{
    mStreamToDecode = nullptr;
    mDecodeMethod = nullptr;
    mInternalState = PDFHummus::eFailure;
}

InputPFBDecodeStream::~InputPFBDecodeStream()
{
    delete mStreamToDecode;
}

EStatusCode InputPFBDecodeStream::Assign(IByteReader *inStreamToDecode)
{
    mStreamToDecode = inStreamToDecode;

    if (mStreamToDecode != nullptr)
    {
        ResetReadStatus();
        mInternalState = InitializeStreamSegment();
        return mInternalState;
    }

    mInternalState = PDFHummus::eFailure;
    return PDFHummus::eSuccess;
}

void InputPFBDecodeStream::ResetReadStatus()
{
    mInSegmentReadIndex = 0;
    mSegmentSize = 0;
    mCurrentType = 0;
    mHasTokenBuffer = false;
    mFoundEOF = false;
    mInternalState = (mStreamToDecode != nullptr) ? PDFHummus::eSuccess : PDFHummus::eFailure;
}

EStatusCode STATIC_NoDecodeRead(InputPFBDecodeStream *inThis, uint8_t &outByte)
{
    return inThis->ReadRegularByte(outByte);
}

EStatusCode InputPFBDecodeStream::ReadRegularByte(uint8_t &outByte)
{
    if (mInSegmentReadIndex >= mSegmentSize)
    {
        return PDFHummus::eFailure;
    }

    ++mInSegmentReadIndex;
    return (mStreamToDecode->Read(&outByte, 1) != 1) ? PDFHummus::eFailure : PDFHummus::eSuccess;
}

EStatusCode InputPFBDecodeStream::InitializeStreamSegment()
{
    EStatusCode status = PDFHummus::eSuccess;
    uint8_t buffer;
    bool requireSegmentReread = false;

    do
    {
        if (!mStreamToDecode->NotEnded())
            break;

        // verify segment header
        if (mStreamToDecode->Read(&buffer, 1) != 1)
        {
            status = PDFHummus::eFailure;
            TRACE_LOG("InputPFBDecodeStream::InitializeStreamSegment, unable to read segment header");
            break;
        }

        if (buffer != 0x80)
        {
            TRACE_LOG("InputPFBDecodeStream::InitializeStreamSegment, wrong segment header");
            break;
        }

        // retrieve segment type
        if (mStreamToDecode->Read(&buffer, 1) != 1)
        {
            status = PDFHummus::eFailure;
            TRACE_LOG("InputPFBDecodeStream::InitializeStreamSegment, unable to read segment type");
            break;
        }

        mInSegmentReadIndex = 0;
        switch (buffer)
        {
        case 1: {
            status = StoreSegmentLength();
            if (status != PDFHummus::eSuccess)
                break;
            mDecodeMethod = STATIC_NoDecodeRead;
            // if previous type was binary, flush also the trailing 0's and cleartomark
            if (2 == mCurrentType)
            {
                status = FlushBinarySectionTrailingCode();
                if (status != PDFHummus::eSuccess)
                    break;

                // flushing might lead us to section end...to save another call
                // check if done here and reread
                if (mSegmentSize <= mInSegmentReadIndex)
                    requireSegmentReread = true;
            }
            break;
        }

        case 2: {
            status = StoreSegmentLength();
            if (status != PDFHummus::eSuccess)
                break;
            status = InitializeBinaryDecode();
            break;
        }

        case 3: {
            mFoundEOF = true;
            break;
        }
        default: {
            TRACE_LOG1("InputPFBDecodeStream::InitializeStreamSegment, unrecognized segment type - %d", buffer);
            status = PDFHummus::eFailure;
            break;
        }
        }
        mCurrentType = buffer;
    } while (false);

    if (PDFHummus::eSuccess == status && requireSegmentReread)
        return InitializeStreamSegment();
    return status;
}

EStatusCode InputPFBDecodeStream::StoreSegmentLength()
{
    uint8_t byte1, byte2, byte3, byte4;

    if (mStreamToDecode->Read(&byte1, 1) != 1)
        return PDFHummus::eFailure;
    if (mStreamToDecode->Read(&byte2, 1) != 1)
        return PDFHummus::eFailure;
    if (mStreamToDecode->Read(&byte3, 1) != 1)
        return PDFHummus::eFailure;
    if (mStreamToDecode->Read(&byte4, 1) != 1)
        return PDFHummus::eFailure;

    mSegmentSize = byte1 | (byte2 << 8) | (byte3 << 16) | (byte4 << 24);
    return PDFHummus::eSuccess;
}

EStatusCode InputPFBDecodeStream::FlushBinarySectionTrailingCode()
{
    int zeroesCount = 512;
    EStatusCode status = PDFHummus::eSuccess;
    uint8_t buffer = 0;

    // skip 0's
    while (zeroesCount > 0 && PDFHummus::eSuccess == status)
    {
        status = ReadRegularByte(buffer);
        if ('0' == buffer)
            --zeroesCount;
    }

    // now skip the final cleartomark token
    BoolAndString nextToken = GetNextToken();
    if (!nextToken.first)
        return PDFHummus::eFailure;

    if (strcmp(nextToken.second.c_str(), "cleartomark") != 0)
        return PDFHummus::eFailure;

    // skip till next token or end of stream
    SkipTillToken();

    return status;
}

static const uint8_t scWhiteSpaces[] = {0, 0x9, 0xA, 0xC, 0xD, 0x20};
bool InputPFBDecodeStream::IsPostScriptWhiteSpace(uint8_t inCharacter)
{
    bool isWhiteSpace = false;
    for (int i = 0; i < 6 && !isWhiteSpace; ++i)
        isWhiteSpace = (scWhiteSpaces[i] == inCharacter);
    return isWhiteSpace;
}

static const uint8_t scEntityBreakers[] = {'(', ')', '<', '>', ']', '[', '{', '}', '/', '%'};
bool InputPFBDecodeStream::IsPostScriptEntityBreaker(uint8_t inCharacter)
{
    bool isEntityBreak = false;
    for (int i = 0; i < 10 && !isEntityBreak; ++i)
        isEntityBreak = (scEntityBreakers[i] == inCharacter);
    return isEntityBreak;
}

static const uint8_t scBackSlash[] = {'\\'};
BoolAndString InputPFBDecodeStream::GetNextToken()
{
    BoolAndString result;
    uint8_t buffer;
    OutputStringBufferStream tokenBuffer;

    if (mInternalState != PDFHummus::eSuccess)
    {
        result.first = false;
        return result;
    }

    // EOF, no token
    if (!NotEnded())
    {
        result.first = false;
        return result;
    }

    // At previous segment end, try a new one
    if (!IsSegmentNotEnded())
    {
        mInternalState = InitializeStreamSegment();
        // new segment brought to end...mark as no token
        if (mInternalState != PDFHummus::eSuccess || !NotEnded())
        {
            result.first = false;
            return result;
        }
    }

    result.first = true;

    do
    {
        // skip till token
        SkipTillToken();

        // if segment ended, mark as no token read
        if (!IsSegmentNotEnded())
        {
            result.first = false;
            break;
        }

        // now read token until it's done. there are some special cases detemining when a token is done
        // based on the first charachter of the token [literal string, hex string , comment]

        if (GetNextByteForToken(buffer) != PDFHummus::eSuccess)
        {
            result.first = false;
            break;
        }

        tokenBuffer.Write(&buffer, 1);
        switch (buffer)
        {
        case '%': {
            // for a comment, the token goes on till the end of line marker [not including]
            while (IsSegmentNotEnded())
            {
                if (GetNextByteForToken(buffer) != PDFHummus::eSuccess)
                {
                    result.first = false;
                    break;
                }
                if (0xD == buffer || 0xA == buffer)
                    break;
                tokenBuffer.Write(&buffer, 1);
            }
            result.second = tokenBuffer.ToString();
            break;
        }

        case '(': {
            // for a string, the token goes on until the balanced-closing right paranthesis
            int balanceLevel = 1;
            bool backSlashEncountered = false;
            while (balanceLevel > 0 && IsSegmentNotEnded())
            {
                if (GetNextByteForToken(buffer) != PDFHummus::eSuccess)
                {
                    result.first = false;
                    break;
                }

                if (backSlashEncountered)
                {
                    backSlashEncountered = false;
                    if (0xA == buffer || 0xD == buffer)
                    {
                        // ignore backslash and newline. might also need to read extra
                        // for cr-ln
                        if (0xD == buffer && IsSegmentNotEnded())
                        {
                            if (GetNextByteForToken(buffer) != PDFHummus::eSuccess)
                            {
                                result.first = false;
                                break;
                            }
                            if (buffer != 0xA)
                                SaveTokenBuffer(buffer);
                        }
                    }
                    else
                    {
                        tokenBuffer.Write(scBackSlash, 1);
                        tokenBuffer.Write(&buffer, 1);
                    }
                }
                else
                {
                    if ('\\' == buffer)
                    {
                        backSlashEncountered = true;
                        continue;
                    }
                    if ('(' == buffer)
                        ++balanceLevel;
                    else if (')' == buffer)
                        --balanceLevel;
                    tokenBuffer.Write(&buffer, 1);
                }
            }
            if (result.first)
                result.second = tokenBuffer.ToString();
            break;
        }

        case '<': {
            if (!IsSegmentNotEnded())
            {
                result.second = tokenBuffer.ToString();
                break;
            }

            // this is either a ASCII base 85 string or a hex string
            // for the purpose of the tokanizer it needs to know if this is a
            // hex string, so as to ignore spaces (not gonna do making sure it's hex
            // leave that to the primitive reader, if one exists)
            if (GetNextByteForToken(buffer) != PDFHummus::eSuccess)
            {
                result.first = false;
                break;
            }

            tokenBuffer.Write(&buffer, 1);
            if ('~' == buffer)
            {
                // ASCII 85 string, read all till '~>'
                while (IsSegmentNotEnded())
                {
                    if (GetNextByteForToken(buffer) != PDFHummus::eSuccess)
                    {
                        result.first = false;
                        break;
                    }

                    tokenBuffer.Write(&buffer, 1);
                    if ('~' == buffer)
                    {
                        if (!IsSegmentNotEnded())
                            break;
                        if (GetNextByteForToken(buffer) != PDFHummus::eSuccess)
                        {
                            result.first = false;
                            break;
                        }
                        tokenBuffer.Write(&buffer, 1);
                        if ('>' == buffer)
                            break;
                    }
                }
                result.second = tokenBuffer.ToString();
            }
            else
            {
                // regular ascii, read anything till '>' skipping white spaces
                while (IsSegmentNotEnded())
                {
                    if (GetNextByteForToken(buffer) != PDFHummus::eSuccess)
                    {
                        result.first = false;
                        break;
                    }

                    if (!IsPostScriptWhiteSpace(buffer))
                        tokenBuffer.Write(&buffer, 1);
                    if ('>' == buffer)
                        break;
                }
                result.second = tokenBuffer.ToString();
            }
            break;
        }
        case '[': // for all array or executable tokanizers, the tokanizer is just the mark
        case ']':
        case '{':
        case '}':
            result.second = tokenBuffer.ToString();
            break;

        default: // regular token. read till next breaker or whitespace
        {
            while (IsSegmentNotEnded())
            {
                if (GetNextByteForToken(buffer) != PDFHummus::eSuccess)
                {
                    result.first = false;
                    break;
                }
                if (IsPostScriptWhiteSpace(buffer))
                {
                    break;
                }
                if (IsPostScriptEntityBreaker(buffer))
                {
                    SaveTokenBuffer(buffer); // for a non-space breaker, save the token for next token read
                    break;
                }
                tokenBuffer.Write(&buffer, 1);
            }
            result.second = tokenBuffer.ToString();
            break;
        }
        }

    } while (false);
    return result;
}

EStatusCode InputPFBDecodeStream::GetNextByteForToken(uint8_t &outByte)
{
    if (mHasTokenBuffer)
    {
        outByte = mTokenBuffer;
        mHasTokenBuffer = false;
        return PDFHummus::eSuccess;
    }
    return mDecodeMethod(this, outByte);
}

void InputPFBDecodeStream::SaveTokenBuffer(uint8_t inToSave)
{
    mHasTokenBuffer = true;
    mTokenBuffer = inToSave;
}

bool InputPFBDecodeStream::IsSegmentNotEnded()
{
    return mHasTokenBuffer || (mInSegmentReadIndex < mSegmentSize && mStreamToDecode->NotEnded());
}

void InputPFBDecodeStream::SkipTillToken()
{
    uint8_t buffer = 0;

    if (mInternalState != PDFHummus::eSuccess || !NotEnded())
        return;

    // skip till hitting first non space, or segment end
    while (IsSegmentNotEnded())
    {
        if (GetNextByteForToken(buffer) != PDFHummus::eSuccess)
            break;

        if (!IsPostScriptWhiteSpace(buffer))
        {
            SaveTokenBuffer(buffer);
            break;
        }
    }
}

EStatusCode STATIC_DecodeRead(InputPFBDecodeStream *inThis, uint8_t &outByte)
{
    return inThis->ReadDecodedByte(outByte);
}

static const int CONSTANT_1 = 52845;
static const int CONSTANT_2 = 22719;
static const int RANDOMIZER_INIT = 55665;
static const int RANDOMIZER_MODULU_VAL = 65536;

EStatusCode InputPFBDecodeStream::InitializeBinaryDecode()
{
    uint8_t dummyByte;
    EStatusCode status = PDFHummus::eSuccess;

    mDecodeMethod = STATIC_DecodeRead;
    mRandomizer = RANDOMIZER_INIT;

    // decode first 4 bytes, which are just a prefix
    for (int i = 0; i < 4 && (PDFHummus::eSuccess == status); ++i)
        status = ReadDecodedByte(dummyByte);
    return status;
}

EStatusCode InputPFBDecodeStream::ReadDecodedByte(uint8_t &outByte)
{
    uint8_t buffer;

    if (mInSegmentReadIndex >= mSegmentSize)
    {
        return PDFHummus::eFailure;
    }

    ++mInSegmentReadIndex;
    if (mStreamToDecode->Read(&buffer, 1) != 1)
        return PDFHummus::eFailure;

    outByte = DecodeByte(buffer);
    return PDFHummus::eSuccess;
}

uint8_t InputPFBDecodeStream::DecodeByte(uint8_t inByteToDecode)
{
    auto result = (uint8_t)(inByteToDecode ^ (mRandomizer >> 8));
    mRandomizer = (uint16_t)(((inByteToDecode + mRandomizer) * CONSTANT_1 + CONSTANT_2) % RANDOMIZER_MODULU_VAL);
    return result;
}

size_t InputPFBDecodeStream::Read(uint8_t *inBuffer, size_t inBufferSize)
{
    size_t bufferIndex = 0;

    if (mHasTokenBuffer && inBufferSize > 0)
    {
        inBuffer[0] = mTokenBuffer;
        mHasTokenBuffer = false;
        ++bufferIndex;
    }

    while (NotEnded() && inBufferSize > bufferIndex && PDFHummus::eSuccess == mInternalState)
    {
        while (mSegmentSize > mInSegmentReadIndex && inBufferSize > bufferIndex &&
               PDFHummus::eSuccess == mInternalState)
        {
            mInternalState = mDecodeMethod(this, inBuffer[bufferIndex]);
            ++bufferIndex;
        }

        // segment ended, initialize next segment
        if (inBufferSize > bufferIndex && NotEnded() && PDFHummus::eSuccess == mInternalState)
            mInternalState = InitializeStreamSegment();
    }
    return bufferIndex;
}

bool InputPFBDecodeStream::NotEnded()
{
    return (mStreamToDecode != nullptr) && mStreamToDecode->NotEnded() && !mFoundEOF;
}

EStatusCode InputPFBDecodeStream::GetInternalState()
{
    return mInternalState;
}