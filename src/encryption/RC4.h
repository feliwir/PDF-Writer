/*
Source File : RC4.h


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

#include <list>
#include <stdint.h>
#include <stdio.h>

typedef std::list<uint8_t> ByteList;

class RC4
{
  public:
    RC4();
    RC4(const ByteList &inKey);
    RC4(const uint8_t *inKey, size_t inLength);
    ~RC4(void);

    void Reset(const ByteList &inKey);
    void Reset(const uint8_t *inKey, size_t inLength);

    uint8_t DecodeNextByte(uint8_t inByte);
    uint8_t GetNextEncodingByte();

  private:
    uint8_t mBuffer[256];
    int mI;
    int mJ;

    void Init(const uint8_t *inKey, size_t inLength);
    void Swap(int a, int b);
};