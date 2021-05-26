/*
   Source File : InputBufferedStream.h


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
#include <vector>
#include <memory>

constexpr size_t DEFAULT_INPUT_BUFFER_SIZE = 256 * 1024;

class InputBufferedStream final : public IByteReaderWithPosition
{
  public:
    /*
        default constructor with default buffer size
    */
    InputBufferedStream();


    /*
        consturctor with buffer size setup
    */
    InputBufferedStream(size_t inBufferSize);

    /*
        Constructor with assigning. see Assign for unassign instructions
    */
    InputBufferedStream(std::unique_ptr<IByteReaderWithPosition> inSourceReader, size_t inBufferSize = DEFAULT_INPUT_BUFFER_SIZE);

    /*
        Assigns a reader stream for buffered reading. from the moment of assigning the
        buffer assumes control of the stream.
        Assign a NULL or a different reader to release ownership.
    */
    void Assign(std::unique_ptr<IByteReaderWithPosition> inReader);

    // IByteReaderWithPosition implementation
    virtual size_t Read(uint8_t *inBuffer, size_t inBufferSize);
    virtual bool NotEnded();
    virtual void Skip(size_t inSkipSize);
    virtual void SetPosition(long long inOffsetFromStart);
    virtual void SetPositionFromEnd(long long inOffsetFromEnd);
    virtual long long GetCurrentPosition();

    IByteReaderWithPosition *GetSourceStream();

  private:
    std::vector<uint8_t> mBuffer;
    size_t mCurrentBufferIndex;
    size_t mLastAvailableIndex;
    std::unique_ptr<IByteReaderWithPosition> mSourceStream;

    void Initiate(std::unique_ptr<IByteReaderWithPosition> inSourceReader, size_t inBufferSize);
};
