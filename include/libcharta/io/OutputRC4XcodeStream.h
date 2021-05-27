/*
Source File : OutputRC4XcodeStream.h


Copyright 2016 Gal Kahana PDFWriter

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
#include "IByteWriterWithPosition.h"
#include "encryption/RC4.h"

#include <list>

namespace charta
{
using ByteList = std::list<uint8_t>;

class OutputRC4XcodeStream final : public IByteWriterWithPosition
{
  public:
    OutputRC4XcodeStream();
    virtual ~OutputRC4XcodeStream();

    OutputRC4XcodeStream(IByteWriterWithPosition *inTargetStream, const ByteList &inEncryptionKey, bool inOwnsStream);

    virtual size_t Write(const uint8_t *inBuffer, size_t inSize);
    virtual long long GetCurrentPosition();

  private:
    bool mOwnsStream;
    IByteWriterWithPosition *mTargetStream;
    RC4 mRC4;
};
} // namespace charta