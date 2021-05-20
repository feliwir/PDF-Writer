/*
   Source File : OutputBufferedStream.h


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

#include "IByteWriterWithPosition.h"

#define DEFAULT_BUFFER_SIZE 256 * 1024

class OutputBufferedStream : public IByteWriterWithPosition
{
  public:
    /*
        default constructor with default buffer size
    */
    OutputBufferedStream(void);

    /*
        Destroys an owned buffer, flushes the buffer before releasing
    */
    virtual ~OutputBufferedStream(void);

    /*
        consturctor with buffer size setup
    */
    OutputBufferedStream(size_t inBufferSize);

    /*
        Constructor with assigning. see Assign for unassign instructions
    */
    OutputBufferedStream(IByteWriterWithPosition *inTargetWriter, size_t inBufferSize = DEFAULT_BUFFER_SIZE);

    /*
        Assigns a writer for buffered writing. from the moment of assigning the
        buffer assumes control of the stream.
        Assign a NULL or a different writer to release ownership.
        replacing a current stream automatically Flushes the buffer.
    */
    void Assign(IByteWriterWithPosition *inWriter);

    // IByteWriter implementation
    virtual size_t Write(const uint8_t *inBuffer, size_t inSize);

    // IByteWriterWithPosition implementation
    virtual long long GetCurrentPosition();

    // force buffer flush to underlying stream
    void Flush();

  private:
    uint8_t *mBuffer;
    size_t mBufferSize;
    uint8_t *mCurrentBufferIndex;
    IByteWriterWithPosition *mTargetStream;

    void Initiate(IByteWriterWithPosition *inTargetWriter, size_t inBufferSize);
};
