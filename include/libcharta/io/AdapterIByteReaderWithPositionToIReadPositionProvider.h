/*
   Source File : AdapterIByteReaderWithPositionToIReadPositionProvider.h


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

#include "IByteReaderWithPosition.h"
#include "IReadPositionProvider.h"

class AdapterIByteReaderWithPositionToIReadPositionProvider : public IReadPositionProvider
{
  public:
    AdapterIByteReaderWithPositionToIReadPositionProvider()
    {
        mStream = NULL;
    }
    AdapterIByteReaderWithPositionToIReadPositionProvider(charta::IByteReaderWithPosition *inStream)
    {
        mStream = inStream;
    }

    void Assign(charta::IByteReaderWithPosition *inStream)
    {
        mStream = inStream;
    }

    // IReadPositionProvider implementation
    virtual long long GetCurrentPosition()
    {
        return mStream->GetCurrentPosition();
    };

  private:
    charta::IByteReaderWithPosition *mStream;
};