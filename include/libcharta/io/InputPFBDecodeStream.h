/*
   Source File : InputPFBDecodeStream.h


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
#pragma once

/*
    reads a PFB stream, decoding the eexec parts. A type 1 parser can use this stream to make the parsing
    more trivial, not requiring to keep track on which area one's in.
*/

#include "EStatusCode.h"
#include "IByteReader.h"

#include <functional>
#include <string>
#include <utility>

typedef std::pair<bool, std::string> BoolAndString;

namespace charta
{
class InputPFBDecodeStream final : public IByteReader
{
  public:
    InputPFBDecodeStream(void);
    ~InputPFBDecodeStream(void);

    // Assign will set the stream to decode. it also takes ownership of the stream. if you
    // don't want the ownership make sure to Assign(NULL) when done using the decoder.
    EStatusCode Assign(IByteReader *inStreamToDecode);

    // IByteReader implementation
    virtual size_t Read(uint8_t *inBuffer, size_t inBufferSize);
    virtual bool NotEnded();

    // token actions

    // get the next avialable postscript token. return result returns whether
    // token retreive was successful and the token (if it was).
    // note that segment end automatically cuts of a token
    BoolAndString GetNextToken();

    // skip white spaces till token or EOF. note that end of segment
    // will stop tokenizer as well
    void SkipTillToken();

    EStatusCode GetInternalState();

    // internal usage.
    EStatusCode ReadDecodedByte(uint8_t &outByte);
    EStatusCode ReadRegularByte(uint8_t &outByte);

  private:
    using DecodeMethod = std::function<EStatusCode(uint8_t &out)>;
    DecodeMethod mDecodeMethod;

    IByteReader *mStreamToDecode;
    long long mInSegmentReadIndex;
    long long mSegmentSize;
    uint8_t mCurrentType;
    bool mHasTokenBuffer;
    uint8_t mTokenBuffer;
    uint16_t mRandomizer;
    bool mFoundEOF;

    // error flag. if set, will not allow further reading
    charta::EStatusCode mInternalState;

    // Starts reading a stream segment, to receive the type and length of the segment
    charta::EStatusCode InitializeStreamSegment();
    void ResetReadStatus();
    charta::EStatusCode StoreSegmentLength();
    charta::EStatusCode FlushBinarySectionTrailingCode();
    bool IsPostScriptWhiteSpace(uint8_t inCharacter);
    bool IsSegmentNotEnded();
    void SaveTokenBuffer(uint8_t inToSave);
    bool IsPostScriptEntityBreaker(uint8_t inCharacter);
    charta::EStatusCode InitializeBinaryDecode();
    uint8_t DecodeByte(uint8_t inByteToDecode);

    charta::EStatusCode GetNextByteForToken(uint8_t &outByte);
};
} // namespace charta