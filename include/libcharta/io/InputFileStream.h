/*
   Source File : InputFileStream.h


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
#include "IByteReaderWithPosition.h"

#include <stdio.h>
#include <string>
#ifdef __MINGW32__
#include <share.h>
#endif

class InputFileStream final : public IByteReaderWithPosition
{
  public:
    InputFileStream() = default;
    virtual ~InputFileStream(void);

    // input file path is in UTF8
    InputFileStream(const std::string &inFilePath);

    // input file path is in UTF8
    charta::EStatusCode Open(const std::string &inFilePath);
    charta::EStatusCode Close();

    // IByteReaderWithPosition implementation
    virtual size_t Read(uint8_t *inBuffer, size_t inBufferSize);
    virtual bool NotEnded();
    virtual void Skip(size_t inSkipSize);
    virtual void SetPosition(long long inOffsetFromStart);
    virtual void SetPositionFromEnd(long long inOffsetFromEnd);
    virtual long long GetCurrentPosition();

    long long GetFileSize();

  private:
    FILE *mStream = nullptr;
};
