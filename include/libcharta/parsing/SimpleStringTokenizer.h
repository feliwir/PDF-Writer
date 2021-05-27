/*
   Source File : PDFParserTokenizer.h


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
#include <stdint.h>
#include <stdio.h>

#include <string>
#include <utility>

class IByteReader;

typedef std::pair<bool, std::string> BoolAndString;

class SimpleStringTokenizer
{
  public:
    SimpleStringTokenizer(void);
    ~SimpleStringTokenizer(void) = default;

    void SetReadStream(IByteReader *inSourceStream);
    BoolAndString GetNextToken();
    void ResetReadState();
    void ResetReadState(const SimpleStringTokenizer &inExternalTokenizer);
    long long GetRecentTokenPosition() const;
    long long GetReadBufferSize() const;

  private:
    IByteReader *mStream;
    bool mHasTokenBuffer;
    uint8_t mTokenBuffer;
    long long mStreamPositionTracker;
    long long mRecentTokenPosition;

    void SkipTillToken();

    // failure in GetNextByteForToken actually marks a true read failure, if you checked end of file before calling
    // it...
    charta::EStatusCode GetNextByteForToken(uint8_t &outByte);

    bool IsPDFWhiteSpace(uint8_t inCharacter);
    void SaveTokenBuffer(uint8_t inToSave);
    bool IsPDFEntityBreaker(uint8_t inCharacter);
};
