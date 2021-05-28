/*
   Source File : PDFObjectParser.h


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

#include "EStatusCode.h"
#include "PDFParserTokenizer.h"
#include "io/IReadPositionProvider.h"
#include <stdint.h>
#include <stdio.h>

#include <list>
#include <memory>
#include <string>
#include <utility>

namespace charta
{
class IByteReader;
class PDFObject;
}; // namespace charta
class IPDFParserExtender;
class DecryptionHelper;

typedef std::pair<bool, uint8_t> BoolAndByte;

class PDFObjectParser
{
  public:
    PDFObjectParser(void);
    ~PDFObjectParser(void);

    // Assign the stream to read from (does not take ownership of the stream, unless told so)
    void SetReadStream(charta::IByteReader *inSourceStream, IReadPositionProvider *inCurrentPositionProvider,
                       bool inOwnsStream = false);

    // the important bit - get next object in content stream
    std::shared_ptr<charta::PDFObject> ParseNewObject();

    // calls this when changing underlying stream position
    void ResetReadState();
    // reset, but with an external tokenizer, which is useful if you are using
    // another pdf parser tokenizer and want to start parsing objects, and don't want to lose
    // buffered characters
    void ResetReadState(const PDFParserTokenizer &inExternalTokenizer);

    void SetDecryptionHelper(DecryptionHelper *inDecryptionHelper);
    void SetParserExtender(IPDFParserExtender *inParserExtender);

    // helper method for others who need to parse encoded pdf data
    std::string DecodeHexString(const std::string &inStringToDecode);

    // External reading. use to temporarily get access to the internal stream, instead of reading objects with
    // ParseNewObject. when done mark with FinishExternalReading to commence reading
    charta::IByteReader *StartExternalRead();
    void EndExternalRead();

  private:
    PDFParserTokenizer mTokenizer;
    std::list<std::string> mTokenBuffer;
    charta::IByteReader *mStream;
    IReadPositionProvider *mCurrentPositionProvider;
    IPDFParserExtender *mParserExtender;
    DecryptionHelper *mDecryptionHelper;
    bool mOwnsStream;

    bool GetNextToken(std::string &outToken);
    void SaveTokenToBuffer(std::string &inToken);
    void ReturnTokenToBuffer(std::string &inToken);

    bool IsBoolean(const std::string &inToken);
    std::shared_ptr<charta::PDFObject> ParseBoolean(const std::string &inToken);

    bool IsLiteralString(const std::string &inToken);
    std::shared_ptr<charta::PDFObject> ParseLiteralString(const std::string &inToken);

    bool IsHexadecimalString(const std::string &inToken);
    std::shared_ptr<charta::PDFObject> ParseHexadecimalString(const std::string &inToken);

    bool IsNull(const std::string &inToken);

    bool IsName(const std::string &inToken);
    std::shared_ptr<charta::PDFObject> ParseName(const std::string &inToken);

    bool IsNumber(const std::string &inToken);
    std::shared_ptr<charta::PDFObject> ParseNumber(const std::string &inToken);

    bool IsArray(const std::string &inToken);
    std::shared_ptr<charta::PDFObject> ParseArray();

    bool IsDictionary(const std::string &inToken);
    std::shared_ptr<charta::PDFObject> ParseDictionary();

    bool IsComment(const std::string &inToken);

    BoolAndByte GetHexValue(uint8_t inValue);

    std::string MaybeDecryptString(const std::string &inString);
};
