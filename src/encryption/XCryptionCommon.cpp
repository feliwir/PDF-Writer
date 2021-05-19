/*
Source File : XCryptionCommon.cpp


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
#include "XCryptionCommon.h"
#include "encryption/MD5Generator.h"
#include "encryption/RC4.h"

#include <algorithm>
#include <stdint.h>

using namespace std;

const uint8_t scPaddingFiller[] = {0x28, 0xBF, 0x4E, 0x5E, 0x4E, 0x75, 0x8A, 0x41, 0x64, 0x00, 0x4E,
                                   0x56, 0xFF, 0xFA, 0x01, 0x08, 0x2E, 0x2E, 0x00, 0xB6, 0xD0, 0x68,
                                   0x3E, 0x80, 0x2F, 0x0C, 0xA9, 0xFE, 0x64, 0x53, 0x69, 0x7A};
XCryptionCommon::XCryptionCommon()
{
    for (unsigned char i : scPaddingFiller)
        mPaddingFiller.push_back(i);
}

XCryptionCommon::~XCryptionCommon() = default;

void XCryptionCommon::Setup(bool inUsingAES)
{
    mUsingAES = inUsingAES;
}

void XCryptionCommon::SetupInitialEncryptionKey(const std::string &inUserPassword, uint32_t inRevision,
                                                uint32_t inLength, const ByteList &inO, long long inP,
                                                const ByteList &inFileIDPart1, bool inEncryptMetaData)
{
    ByteList password = stringToByteList(inUserPassword);

    mEncryptionKey = algorithm3_2(inRevision, inLength, password, inO, inP, inFileIDPart1, inEncryptMetaData);
}

void XCryptionCommon::SetupInitialEncryptionKey(const ByteList &inEncryptionKey)
{
    mEncryptionKey = inEncryptionKey;
}

const ByteList &XCryptionCommon::GetInitialEncryptionKey() const
{
    return mEncryptionKey;
}

const ByteList &XCryptionCommon::OnObjectStart(long long inObjectID, long long inGenerationNumber)
{
    mEncryptionKeysStack.push_back(
        ComputeEncryptionKeyForObject((ObjectIDType)inObjectID, (unsigned long)inGenerationNumber));

    return mEncryptionKeysStack.back();
}

ByteList XCryptionCommon::ComputeEncryptionKeyForObject(ObjectIDType inObjectNumber, unsigned long inGenerationNumber)
{
    return algorithm3_1(inObjectNumber, inGenerationNumber, mEncryptionKey, mUsingAES);
}

void XCryptionCommon::OnObjectEnd()
{
    mEncryptionKeysStack.pop_back();
}

const ByteList scEmptyByteList;

const ByteList &XCryptionCommon::GetCurrentObjectKey()
{
    return !mEncryptionKeysStack.empty() ? mEncryptionKeysStack.back() : scEmptyByteList;
}

ByteList XCryptionCommon::stringToByteList(const std::string &inString)
{
    ByteList buffer;
    std::string::const_iterator it = inString.begin();

    for (; it != inString.end(); ++it)
        buffer.push_back((uint8_t)*it);

    return buffer;
}
ByteList XCryptionCommon::substr(const ByteList &inList, size_t inStart, size_t inLength)
{
    ByteList buffer;
    auto it = inList.begin();

    for (size_t i = 0; i < inStart && it != inList.end(); ++i, ++it)
        ;

    for (size_t i = 0; i < inLength && it != inList.end(); ++i, ++it)
        buffer.push_back((uint8_t)*it);

    return buffer;
}

void XCryptionCommon::append(ByteList &ioTargetList, const ByteList &inSource)
{
    auto it = inSource.begin();

    for (; it != inSource.end(); ++it)
        ioTargetList.push_back(*it);
}

ByteList XCryptionCommon::add(const ByteList &inA, const ByteList &inB)
{
    ByteList buffer;

    append(buffer, inA);
    append(buffer, inB);

    return buffer;
}

std::string XCryptionCommon::ByteListToString(const ByteList &inByteList)
{
    std::string buffer;
    auto it = inByteList.begin();

    for (; it != inByteList.end(); ++it)
        buffer.push_back((char)*it);

    return buffer;
}

const uint8_t scAESSuffix[] = {0x73, 0x41, 0x6C, 0x54};
ByteList XCryptionCommon::algorithm3_1(ObjectIDType inObjectNumber, unsigned long inGenerationNumber,
                                       const ByteList &inEncryptionKey, bool inIsUsingAES)
{
    MD5Generator md5;
    ByteList result = inEncryptionKey;
    uint8_t buffer;
    size_t outputKeyLength = std::min<size_t>(inEncryptionKey.size() + 5, 16U);

    buffer = inObjectNumber & 0xff;
    result.push_back(buffer);
    inObjectNumber >>= 8;
    buffer = inObjectNumber & 0xff;
    result.push_back(buffer);
    inObjectNumber >>= 8;
    buffer = inObjectNumber & 0xff;
    result.push_back(buffer);

    buffer = inGenerationNumber & 0xff;
    result.push_back(buffer);
    inGenerationNumber >>= 8;
    buffer = inGenerationNumber & 0xff;
    result.push_back(buffer);

    if (inIsUsingAES)
    {
        for (unsigned char i : scAESSuffix)
        {
            result.push_back(i);
        }
    }
    md5.Accumulate(result);

    return substr(md5.ToString(), 0, outputKeyLength);
}

const uint8_t scFixedEnd[] = {0xFF, 0xFF, 0xFF, 0xFF};
ByteList XCryptionCommon::algorithm3_2(uint32_t inRevision, uint32_t inLength, const ByteList &inPassword,
                                       const ByteList &inO, long long inP, const ByteList &inFileIDPart1,
                                       bool inEncryptMetaData)
{
    MD5Generator md5;
    ByteList password32Chars = substr(inPassword, 0, 32);
    if (password32Chars.size() < 32)
        append(password32Chars, substr(mPaddingFiller, 0, 32 - inPassword.size()));
    auto truncP = uint32_t(inP);
    uint8_t truncPBuffer[4];
    ByteList hashResult;

    md5.Accumulate(password32Chars);
    md5.Accumulate(inO);
    for (unsigned char &i : truncPBuffer)
    {
        i = truncP & 0xFF;
        truncP >>= 8;
    }
    md5.Accumulate(truncPBuffer, 4);
    md5.Accumulate(inFileIDPart1);

    if (inRevision >= 4 && !inEncryptMetaData)
        md5.Accumulate(scFixedEnd, 4);

    hashResult = md5.ToString();

    if (inRevision >= 3)
    {
        for (int i = 0; i < 50; ++i)
        {
            MD5Generator anotherMD5;
            anotherMD5.Accumulate(substr(hashResult, 0, inLength));
            hashResult = anotherMD5.ToString();
        }
    }

    return inRevision == 2 ? substr(hashResult, 0, 5) : substr(hashResult, 0, inLength);
}

ByteList XCryptionCommon::algorithm3_3(uint32_t inRevision, uint32_t inLength, const ByteList &inOwnerPassword,
                                       const ByteList &inUserPassword)
{
    ByteList ownerPassword32Chars =
        add(substr(inOwnerPassword, 0, 32),
            (inOwnerPassword.size() < 32 ? substr(mPaddingFiller, 0, 32 - inOwnerPassword.size()) : ByteList()));
    ByteList userPassword32Chars =
        add(substr(inUserPassword, 0, 32),
            (inUserPassword.size() < 32 ? substr(mPaddingFiller, 0, 32 - inUserPassword.size()) : ByteList()));
    MD5Generator md5;
    ByteList hashResult;

    md5.Accumulate(ownerPassword32Chars);

    hashResult = md5.ToString();

    if (inRevision >= 3)
    {
        for (int i = 0; i < 50; ++i)
        {
            MD5Generator anotherMD5;
            anotherMD5.Accumulate(hashResult);
            hashResult = anotherMD5.ToString();
        }
    }

    ByteList RC4Key = (inRevision == 2 ? substr(hashResult, 0, 5) : substr(hashResult, 0, inLength));

    hashResult = RC4Encode(RC4Key, userPassword32Chars);

    if (inRevision >= 3)
    {
        for (uint8_t i = 1; i <= 19; ++i)
        {
            ByteList newRC4Key;
            auto it = RC4Key.begin();
            for (; it != RC4Key.end(); ++it)
                newRC4Key.push_back((*it) ^ i);
            hashResult = RC4Encode(newRC4Key, hashResult);
        }
    }

    return hashResult;
}

ByteList XCryptionCommon::RC4Encode(const ByteList &inKey, const ByteList &inToEncode)
{
    RC4 rc4(inKey);
    ByteList target;
    uint8_t buffer;
    auto it = inToEncode.begin();

    for (; it != inToEncode.end(); ++it)
    {
        buffer = rc4.DecodeNextByte((uint8_t)*it);
        target.push_back(buffer);
    }
    return target;
}

ByteList XCryptionCommon::algorithm3_4(uint32_t inLength, const ByteList &inUserPassword, const ByteList &inO,
                                       long long inP, const ByteList &inFileIDPart1, bool inEncryptMetaData)
{
    ByteList encryptionKey = algorithm3_2(2, inLength, inUserPassword, inO, inP, inFileIDPart1, inEncryptMetaData);

    return RC4Encode(encryptionKey, mPaddingFiller);
}

ByteList XCryptionCommon::algorithm3_5(uint32_t inRevision, uint32_t inLength, const ByteList &inUserPassword,
                                       const ByteList &inO, long long inP, const ByteList &inFileIDPart1,
                                       bool inEncryptMetaData)
{
    ByteList encryptionKey =
        algorithm3_2(inRevision, inLength, inUserPassword, inO, inP, inFileIDPart1, inEncryptMetaData);
    MD5Generator md5;
    ByteList hashResult;

    md5.Accumulate(mPaddingFiller);
    md5.Accumulate(inFileIDPart1);
    hashResult = md5.ToString();

    hashResult = RC4Encode(encryptionKey, hashResult);

    for (uint8_t i = 1; i <= 19; ++i)
    {
        ByteList newEncryptionKey;
        auto it = encryptionKey.begin();
        for (; it != encryptionKey.end(); ++it)
            newEncryptionKey.push_back((*it) ^ i);
        hashResult = RC4Encode(newEncryptionKey, hashResult);
    }

    return add(hashResult, substr(mPaddingFiller, 0, 16));
}

bool XCryptionCommon::algorithm3_6(uint32_t inRevision, uint32_t inLength, const ByteList &inPassword,
                                   const ByteList &inO, long long inP, const ByteList &inFileIDPart1,
                                   bool inEncryptMetaData, const ByteList &inU)
{
    ByteList hashResult =
        (inRevision == 2) ? algorithm3_4(inLength, inPassword, inO, inP, inFileIDPart1, inEncryptMetaData)
                          : algorithm3_5(inRevision, inLength, inPassword, inO, inP, inFileIDPart1, inEncryptMetaData);

    return (inRevision == 2) ? (hashResult == inU) : (substr(hashResult, 0, 16) == substr(inU, 0, 16));
}

bool XCryptionCommon::algorithm3_7(uint32_t inRevision, uint32_t inLength, const ByteList &inPassword,
                                   const ByteList &inO, long long inP, const ByteList &inFileIDPart1,
                                   bool inEncryptMetaData, const ByteList &inU)
{
    ByteList password32Chars =
        add(substr(inPassword, 0, 32),
            (inPassword.size() < 32 ? substr(mPaddingFiller, 0, 32 - inPassword.size()) : ByteList()));
    MD5Generator md5;
    ByteList hashResult;

    md5.Accumulate(password32Chars);

    hashResult = md5.ToString();

    if (inRevision >= 3)
    {
        for (int i = 0; i < 50; ++i)
        {
            MD5Generator anotherMD5;
            anotherMD5.Accumulate(hashResult);
            hashResult = anotherMD5.ToString();
        }
    }

    ByteList RC4Key = (inRevision == 2 ? substr(hashResult, 0, 5) : substr(hashResult, 0, inLength));

    if (inRevision == 2)
    {
        hashResult = RC4Encode(RC4Key, inO);
    }
    else if (inRevision >= 3)
    {
        hashResult = inO;

        for (int i = 19; i >= 0; --i)
        {
            ByteList newEncryptionKey;
            auto it = RC4Key.begin();
            for (; it != RC4Key.end(); ++it)
                newEncryptionKey.push_back((*it) ^ i);

            hashResult = RC4Encode(newEncryptionKey, hashResult);
        }
    }

    return algorithm3_6(inRevision, inLength, hashResult, inO, inP, inFileIDPart1, inEncryptMetaData, inU);
}

bool XCryptionCommon::IsUsingAES() const
{
    return mUsingAES;
}