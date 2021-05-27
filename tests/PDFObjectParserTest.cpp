/*
Source File : PDFObjectParserTest.cpp


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
#include "parsing/PDFObjectParser.h"
#include "PDFTextString.h"
#include "TestHelper.h"
#include "io/IByteWriterWithPosition.h"
#include "io/OutputFile.h"
#include "objects/PDFHexString.h"
#include "objects/PDFLiteralString.h"
#include "objects/PDFObject.h"
#include "objects/PDFObjectCast.h"
#include "objects/helpers/ParsedPrimitiveHelper.h"
#include "parsing/PDFParser.h"

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

using namespace charta;

class InputInterfaceToStream : public IByteReader, public IReadPositionProvider
{
  private:
    std::string mInput;
    std::string::size_type mLen, mPos;

  public:
    size_t Read(uint8_t *inBuffer, size_t inBufferSize) override
    {
        const char *data = mInput.data();
        if (!NotEnded())
            return 0;
        size_t size = mLen - mPos;
        if (size > inBufferSize)
            size = inBufferSize;
        memcpy(inBuffer, &data[mPos], size);
        mPos += size;
        return size;
    }

    bool NotEnded() override
    {
        return mPos < mLen;
    };

    long long GetCurrentPosition() override
    {
        return static_cast<long long>(mPos);
    };

    void setInput(const std::string &input)
    {
        mInput = input;
        mLen = input.size();
        mPos = 0;
    }

    const std::string &getInput() const
    {
        return mInput;
    }

    InputInterfaceToStream() : mLen(0), mPos(0)
    {
    }
};

class PDFObjectParserTestLogHelper
{
  private:
    OutputFile mOutput;

  public:
    EStatusCode openLog(const std::string &path)
    {
        return mOutput.OpenFile(path);
    }

    template <class T> PDFObjectParserTestLogHelper &operator<<(T out)
    {
        std::ostringstream ss;
        ss << out;
        mOutput.GetOutputStream()->Write((const unsigned char *)ss.str().c_str(), ss.str().size());
        return *this;
    }
};

template <class _resultObjectType> class ExpectedResult
{
  private:
    std::shared_ptr<PDFObject> mObject;
    std::shared_ptr<_resultObjectType> mTypedObject;

  public:
    ExpectedResult(std::shared_ptr<PDFObject> object) : mObject(object) // ParseNewObject constructs with refcount 1
    {
        mTypedObject = PDFObjectCast<_resultObjectType>(object); // addrefs if valid object
    }

    int setResult(const std::string &input, const std::string &expected, PDFObjectParserTestLogHelper &log)
    {
        if (!mObject)
        {
            log << "Failed parse: no object parsed from input: '" << input << "'";
            return 1;
        }
        if (!!mObject && !mTypedObject)
        {
            int object_type = mObject->GetType();
            std::string result_type = "invalid";
            if (object_type >= 0 && object_type <= PDFObject::ePDFObjectSymbol)
            {
                result_type = PDFObject::scPDFObjectTypeLabel(mObject->GetType());
            }

            log << "Failed parse. Expected '" << PDFObject::scPDFObjectTypeLabel(_resultObjectType::eType) << "' got '"
                << result_type << "' from input: '" << input << "'\n";
            return 1;
        }
        std::string result = PDFTextString(ParsedPrimitiveHelper(mTypedObject).ToString()).ToUTF8String();
        if (result != expected)
        {
            log << "Failed parse. Expected '" << expected << "' got '" << result << "' from input: '" << input << "'\n";
            return 1;
        }
        log << "Parse passed: '" << input << "'\n";
        return 0;
    }
};

EStatusCode ParseCommentedTokens(PDFObjectParser *objectParser, PDFObjectParserTestLogHelper *log)
{
    int failures = 0;
    InputInterfaceToStream input;
    std::shared_ptr<PDFObject> object;

    {
        input.setInput("(val%ami)");
        objectParser->SetReadStream(&input, &input);
        ExpectedResult<PDFLiteralString> result(objectParser->ParseNewObject());

        failures += result.setResult(input.getInput(), "val%ami", *log);
    };

    /*
    // I don't know what this should return. Currently it returns 'x41 x54 x20' which is wrong (parse error?)
    {
        input.setInput("<41%\n42>");
        objectParser->SetReadStream(&input, &input);
        ExpectedResult<PDFHexString> result(objectParser->ParseNewObject());

        failures += result.setResult(input.getInput(), "AB", *log);
    };*/

    return (failures > 0) ? eFailure : eSuccess;
}

EStatusCode ParseHexStringTokens(PDFObjectParser *objectParser, PDFObjectParserTestLogHelper *log)
{
    int failures = 0;
    InputInterfaceToStream input;
    std::shared_ptr<PDFObject> object;

    {
        input.setInput("<>");
        objectParser->SetReadStream(&input, &input);
        ExpectedResult<PDFHexString> result(objectParser->ParseNewObject());

        failures += result.setResult(input.getInput(), "", *log);
    };

    {
        input.setInput("<   >");
        objectParser->SetReadStream(&input, &input);
        ExpectedResult<PDFHexString> result(objectParser->ParseNewObject());

        failures += result.setResult(input.getInput(), "", *log);
    };

    {
        input.setInput("<\n>");
        objectParser->SetReadStream(&input, &input);
        ExpectedResult<PDFHexString> result(objectParser->ParseNewObject());

        failures += result.setResult(input.getInput(), "", *log);
    };

    {
        input.setInput("< 41   4 2 >");
        objectParser->SetReadStream(&input, &input);
        ExpectedResult<PDFHexString> result(objectParser->ParseNewObject());

        failures += result.setResult(input.getInput(), "AB", *log);
    };

    {
        input.setInput("<04000201>");
        objectParser->SetReadStream(&input, &input);
        ExpectedResult<PDFHexString> result(objectParser->ParseNewObject());

        failures += result.setResult(input.getInput(), std::string("\x04\x00\x02\x01", 4), *log);
    };

    {
        input.setInput("<feff00410042>");
        objectParser->SetReadStream(&input, &input);
        ExpectedResult<PDFHexString> result(objectParser->ParseNewObject());

        failures += result.setResult(input.getInput(), "\x41\x42", *log);
    };

    {
        input.setInput("<f e f f 0 04 10042>");
        objectParser->SetReadStream(&input, &input);
        ExpectedResult<PDFHexString> result(objectParser->ParseNewObject());

        failures += result.setResult(input.getInput(), "\x41\x42", *log);
    };

    {
        input.setInput("<5>");
        objectParser->SetReadStream(&input, &input);
        ExpectedResult<PDFHexString> result(objectParser->ParseNewObject());

        failures += result.setResult(input.getInput(), "\x50", *log);
    };

    return (failures > 0) ? eFailure : eSuccess;
}

EStatusCode ParseLiteralStringTokens(PDFObjectParser *objectParser, PDFObjectParserTestLogHelper *log)
{
    int failures = 0;
    InputInterfaceToStream input;
    std::shared_ptr<PDFObject> object;

    {
        input.setInput("()");
        objectParser->SetReadStream(&input, &input);
        ExpectedResult<PDFLiteralString> result(objectParser->ParseNewObject());

        failures += result.setResult(input.getInput(), "", *log);
    };

    {
        input.setInput("( )");
        objectParser->SetReadStream(&input, &input);
        ExpectedResult<PDFLiteralString> result(objectParser->ParseNewObject());

        failures += result.setResult(input.getInput(), " ", *log);
    };

    {
        input.setInput("( \n )");
        objectParser->SetReadStream(&input, &input);
        ExpectedResult<PDFLiteralString> result(objectParser->ParseNewObject());

        failures += result.setResult(input.getInput(), " \n ", *log);
    };
    {
        input.setInput("( \\\n )");
        objectParser->SetReadStream(&input, &input);
        ExpectedResult<PDFLiteralString> result(objectParser->ParseNewObject());

        failures += result.setResult(input.getInput(), "  ", *log);
    };

    {
        input.setInput("( ( ) )");
        objectParser->SetReadStream(&input, &input);
        ExpectedResult<PDFLiteralString> result(objectParser->ParseNewObject());

        failures += result.setResult(input.getInput(), " ( ) ", *log);
    };

    {
        input.setInput("( \\( \\) )");
        objectParser->SetReadStream(&input, &input);
        ExpectedResult<PDFLiteralString> result(objectParser->ParseNewObject());

        failures += result.setResult(input.getInput(), " ( ) ", *log);
    };

    {
        input.setInput("(\\101\\102)"); // in hex: 41 42
        objectParser->SetReadStream(&input, &input);
        ExpectedResult<PDFLiteralString> result(objectParser->ParseNewObject());

        failures += result.setResult(input.getInput(), "AB", *log);
    };

    {
        input.setInput(std::string("(\\376\\377\\0\\101)", 16)); // in hex this is: fe ff 00 41
        objectParser->SetReadStream(&input, &input);
        ExpectedResult<PDFLiteralString> result(objectParser->ParseNewObject());

        failures += result.setResult(input.getInput(), "A", *log);
    };

    return (failures > 0) ? eFailure : eSuccess;
}

TEST(Parsing, PDFObjectParser)
{
    int failures = 0;
    PDFObjectParserTestLogHelper log;
    ASSERT_EQ(log.openLog(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "PDFObjectParserTest.txt")), eSuccess);

    PDFParser parser;
    PDFObjectParser *pObjectParser = &parser.GetObjectParser();
    EXPECT_EQ(ParseCommentedTokens(pObjectParser, &log), eSuccess);
    EXPECT_EQ(ParseHexStringTokens(pObjectParser, &log), eSuccess);
    EXPECT_EQ(ParseLiteralStringTokens(pObjectParser, &log), eSuccess);
}
