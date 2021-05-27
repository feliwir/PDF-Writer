/*
   Source File : CFFFileInput.cpp


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
#include "text/cff/CFFFileInput.h"
#include "Trace.h"
#include "encoding/StandardEncoding.h"
#include "text/cff/CharStringType2Interpreter.h"

using namespace charta;

#define N_STD_STRINGS 391
static const char *scStandardStrings[N_STD_STRINGS] = {".notdef",
                                                       "space",
                                                       "exclam",
                                                       "quotedbl",
                                                       "numbersign",
                                                       "dollar",
                                                       "percent",
                                                       "ampersand",
                                                       "quoteright",
                                                       "parenleft",
                                                       "parenright",
                                                       "asterisk",
                                                       "plus",
                                                       "comma",
                                                       "hyphen",
                                                       "period",
                                                       "slash",
                                                       "zero",
                                                       "one",
                                                       "two",
                                                       "three",
                                                       "four",
                                                       "five",
                                                       "six",
                                                       "seven",
                                                       "eight",
                                                       "nine",
                                                       "colon",
                                                       "semicolon",
                                                       "less",
                                                       "equal",
                                                       "greater",
                                                       "question",
                                                       "at",
                                                       "A",
                                                       "B",
                                                       "C",
                                                       "D",
                                                       "E",
                                                       "F",
                                                       "G",
                                                       "H",
                                                       "I",
                                                       "J",
                                                       "K",
                                                       "L",
                                                       "M",
                                                       "N",
                                                       "O",
                                                       "P",
                                                       "Q",
                                                       "R",
                                                       "S",
                                                       "T",
                                                       "U",
                                                       "V",
                                                       "W",
                                                       "X",
                                                       "Y",
                                                       "Z",
                                                       "bracketleft",
                                                       "backslash",
                                                       "bracketright",
                                                       "asciicircum",
                                                       "underscore",
                                                       "quoteleft",
                                                       "a",
                                                       "b",
                                                       "c",
                                                       "d",
                                                       "e",
                                                       "f",
                                                       "g",
                                                       "h",
                                                       "i",
                                                       "j",
                                                       "k",
                                                       "l",
                                                       "m",
                                                       "n",
                                                       "o",
                                                       "p",
                                                       "q",
                                                       "r",
                                                       "s",
                                                       "t",
                                                       "u",
                                                       "v",
                                                       "w",
                                                       "x",
                                                       "y",
                                                       "z",
                                                       "braceleft",
                                                       "bar",
                                                       "braceright",
                                                       "asciitilde",
                                                       "exclamdown",
                                                       "cent",
                                                       "sterling",
                                                       "fraction",
                                                       "yen",
                                                       "florin",
                                                       "section",
                                                       "currency",
                                                       "quotesingle",
                                                       "quotedblleft",
                                                       "guillemotleft",
                                                       "guilsinglleft",
                                                       "guilsinglright",
                                                       "fi",
                                                       "fl",
                                                       "endash",
                                                       "dagger",
                                                       "daggerdbl",
                                                       "periodcentered",
                                                       "paragraph",
                                                       "bullet",
                                                       "quotesinglbase",
                                                       "quotedblbase",
                                                       "quotedblright",
                                                       "guillemotright",
                                                       "ellipsis",
                                                       "perthousand",
                                                       "questiondown",
                                                       "grave",
                                                       "acute",
                                                       "circumflex",
                                                       "tilde",
                                                       "macron",
                                                       "breve",
                                                       "dotaccent",
                                                       "dieresis",
                                                       "ring",
                                                       "cedilla",
                                                       "hungarumlaut",
                                                       "ogonek",
                                                       "caron",
                                                       "emdash",
                                                       "AE",
                                                       "ordfeminine",
                                                       "Lslash",
                                                       "Oslash",
                                                       "OE",
                                                       "ordmasculine",
                                                       "ae",
                                                       "dotlessi",
                                                       "lslash",
                                                       "oslash",
                                                       "oe",
                                                       "germandbls",
                                                       "onesuperior",
                                                       "logicalnot",
                                                       "mu",
                                                       "trademark",
                                                       "Eth",
                                                       "onehalf",
                                                       "plusminus",
                                                       "Thorn",
                                                       "onequarter",
                                                       "divide",
                                                       "brokenbar",
                                                       "degree",
                                                       "thorn",
                                                       "threequarters",
                                                       "twosuperior",
                                                       "registered",
                                                       "minus",
                                                       "eth",
                                                       "multiply",
                                                       "threesuperior",
                                                       "copyright",
                                                       "Aacute",
                                                       "Acircumflex",
                                                       "Adieresis",
                                                       "Agrave",
                                                       "Aring",
                                                       "Atilde",
                                                       "Ccedilla",
                                                       "Eacute",
                                                       "Ecircumflex",
                                                       "Edieresis",
                                                       "Egrave",
                                                       "Iacute",
                                                       "Icircumflex",
                                                       "Idieresis",
                                                       "Igrave",
                                                       "Ntilde",
                                                       "Oacute",
                                                       "Ocircumflex",
                                                       "Odieresis",
                                                       "Ograve",
                                                       "Otilde",
                                                       "Scaron",
                                                       "Uacute",
                                                       "Ucircumflex",
                                                       "Udieresis",
                                                       "Ugrave",
                                                       "Yacute",
                                                       "Ydieresis",
                                                       "Zcaron",
                                                       "aacute",
                                                       "acircumflex",
                                                       "adieresis",
                                                       "agrave",
                                                       "aring",
                                                       "atilde",
                                                       "ccedilla",
                                                       "eacute",
                                                       "ecircumflex",
                                                       "edieresis",
                                                       "egrave",
                                                       "iacute",
                                                       "icircumflex",
                                                       "idieresis",
                                                       "igrave",
                                                       "ntilde",
                                                       "oacute",
                                                       "ocircumflex",
                                                       "odieresis",
                                                       "ograve",
                                                       "otilde",
                                                       "scaron",
                                                       "uacute",
                                                       "ucircumflex",
                                                       "udieresis",
                                                       "ugrave",
                                                       "yacute",
                                                       "ydieresis",
                                                       "zcaron",
                                                       "exclamsmall",
                                                       "Hungarumlautsmall",
                                                       "dollaroldstyle",
                                                       "dollarsuperior",
                                                       "ampersandsmall",
                                                       "Acutesmall",
                                                       "parenleftsuperior",
                                                       "parenrightsuperior",
                                                       "twodotenleader",
                                                       "onedotenleader",
                                                       "zerooldstyle",
                                                       "oneoldstyle",
                                                       "twooldstyle",
                                                       "threeoldstyle",
                                                       "fouroldstyle",
                                                       "fiveoldstyle",
                                                       "sixoldstyle",
                                                       "sevenoldstyle",
                                                       "eightoldstyle",
                                                       "nineoldstyle",
                                                       "commasuperior",
                                                       "threequartersemdash",
                                                       "periodsuperior",
                                                       "questionsmall",
                                                       "asuperior",
                                                       "bsuperior",
                                                       "centsuperior",
                                                       "dsuperior",
                                                       "esuperior",
                                                       "isuperior",
                                                       "lsuperior",
                                                       "msuperior",
                                                       "nsuperior",
                                                       "osuperior",
                                                       "rsuperior",
                                                       "ssuperior",
                                                       "tsuperior",
                                                       "ff",
                                                       "ffi",
                                                       "ffl",
                                                       "parenleftinferior",
                                                       "parenrightinferior",
                                                       "Circumflexsmall",
                                                       "hyphensuperior",
                                                       "Gravesmall",
                                                       "Asmall",
                                                       "Bsmall",
                                                       "Csmall",
                                                       "Dsmall",
                                                       "Esmall",
                                                       "Fsmall",
                                                       "Gsmall",
                                                       "Hsmall",
                                                       "Ismall",
                                                       "Jsmall",
                                                       "Ksmall",
                                                       "Lsmall",
                                                       "Msmall",
                                                       "Nsmall",
                                                       "Osmall",
                                                       "Psmall",
                                                       "Qsmall",
                                                       "Rsmall",
                                                       "Ssmall",
                                                       "Tsmall",
                                                       "Usmall",
                                                       "Vsmall",
                                                       "Wsmall",
                                                       "Xsmall",
                                                       "Ysmall",
                                                       "Zsmall",
                                                       "colonmonetary",
                                                       "onefitted",
                                                       "rupiah",
                                                       "Tildesmall",
                                                       "exclamdownsmall",
                                                       "centoldstyle",
                                                       "Lslashsmall",
                                                       "Scaronsmall",
                                                       "Zcaronsmall",
                                                       "Dieresissmall",
                                                       "Brevesmall",
                                                       "Caronsmall",
                                                       "Dotaccentsmall",
                                                       "Macronsmall",
                                                       "figuredash",
                                                       "hypheninferior",
                                                       "Ogoneksmall",
                                                       "Ringsmall",
                                                       "Cedillasmall",
                                                       "questiondownsmall",
                                                       "oneeighth",
                                                       "threeeighths",
                                                       "fiveeighths",
                                                       "seveneighths",
                                                       "onethird",
                                                       "twothirds",
                                                       "zerosuperior",
                                                       "foursuperior",
                                                       "fivesuperior",
                                                       "sixsuperior",
                                                       "sevensuperior",
                                                       "eightsuperior",
                                                       "ninesuperior",
                                                       "zeroinferior",
                                                       "oneinferior",
                                                       "twoinferior",
                                                       "threeinferior",
                                                       "fourinferior",
                                                       "fiveinferior",
                                                       "sixinferior",
                                                       "seveninferior",
                                                       "eightinferior",
                                                       "nineinferior",
                                                       "centinferior",
                                                       "dollarinferior",
                                                       "periodinferior",
                                                       "commainferior",
                                                       "Agravesmall",
                                                       "Aacutesmall",
                                                       "Acircumflexsmall",
                                                       "Atildesmall",
                                                       "Adieresissmall",
                                                       "Aringsmall",
                                                       "AEsmall",
                                                       "Ccedillasmall",
                                                       "Egravesmall",
                                                       "Eacutesmall",
                                                       "Ecircumflexsmall",
                                                       "Edieresissmall",
                                                       "Igravesmall",
                                                       "Iacutesmall",
                                                       "Icircumflexsmall",
                                                       "Idieresissmall",
                                                       "Ethsmall",
                                                       "Ntildesmall",
                                                       "Ogravesmall",
                                                       "Oacutesmall",
                                                       "Ocircumflexsmall",
                                                       "Otildesmall",
                                                       "Odieresissmall",
                                                       "OEsmall",
                                                       "Oslashsmall",
                                                       "Ugravesmall",
                                                       "Uacutesmall",
                                                       "Ucircumflexsmall",
                                                       "Udieresissmall",
                                                       "Yacutesmall",
                                                       "Thornsmall",
                                                       "Ydieresissmall",
                                                       "001.000",
                                                       "001.001",
                                                       "001.002",
                                                       "001.003",
                                                       "Black",
                                                       "Bold",
                                                       "Book",
                                                       "Light",
                                                       "Medium",
                                                       "Regular",
                                                       "Roman",
                                                       "Semibold"};

#define CHARSET_ISOADOBE_SIZE 228
static const uint16_t scCharsetIsoadobeSids[CHARSET_ISOADOBE_SIZE] = {
    1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,
    24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,
    47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,
    70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,
    93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115,
    116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138,
    139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161,
    162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184,
    185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228};

#define CHARSET_EXPERT_SIZE 165
static const uint16_t scCharsetExpertSids[CHARSET_EXPERT_SIZE] = {
    1,   229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 13,  14,  15,  99,  239, 240, 241, 242, 243, 244,
    245, 246, 247, 248, 27,  28,  249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263,
    264, 265, 266, 109, 110, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282,
    283, 284, 285, 286, 287, 288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302, 303,
    304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 158, 155, 163, 319, 320, 321,
    322, 323, 324, 325, 326, 150, 164, 169, 327, 328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339,
    340, 341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356, 357, 358, 359, 360,
    361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378};

#define CHARSET_EXPERT_SUBSET_SIZE 86
static const uint16_t scCharsetExpertSubsetSids[CHARSET_EXPERT_SUBSET_SIZE] = {
    1,   231, 232, 235, 236, 237, 238, 13,  14,  15,  99,  239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 27,
    28,  249, 250, 251, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 109, 110, 267, 268,
    269, 270, 272, 300, 301, 302, 305, 314, 315, 158, 155, 163, 320, 321, 322, 323, 324, 325, 326, 150, 164, 169,
    327, 328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342, 343, 344, 345, 346};

static const uint16_t *scDefaultCharsets[3] = {scCharsetIsoadobeSids, scCharsetExpertSids, scCharsetExpertSubsetSids};

static const uint16_t scDefaultCharsetsSizes[3] = {CHARSET_ISOADOBE_SIZE, CHARSET_EXPERT_SIZE,
                                                   CHARSET_EXPERT_SUBSET_SIZE};

CFFFileInput::CFFFileInput()
{
    mTopDictIndex = nullptr;
    mStrings = nullptr;
    mGlobalSubrs.mCharStringsIndex = nullptr;
    mCharStrings = nullptr;
    mPrivateDicts = nullptr;
}

CFFFileInput::~CFFFileInput()
{
    FreeData();
}

void CFFFileInput::FreeData()
{
    mName.clear();
    mNameToIndex.clear();
    if (mTopDictIndex != nullptr)
    {
        for (unsigned long i = 0; i < mFontsCount; ++i)
        {
            delete[] mTopDictIndex[i].mFDArray;
            delete[] mTopDictIndex[i].mFDSelect;
        }
        delete[] mTopDictIndex;
        mTopDictIndex = nullptr;
    }
    if (mStrings != nullptr)
    {
        for (unsigned long i = 0; i < mStringsCount; ++i)
            delete[] mStrings[i];
        delete[] mStrings;
        mStrings = nullptr;
    }
    mStringToSID.clear();
    delete[] mGlobalSubrs.mCharStringsIndex;
    mGlobalSubrs.mCharStringsIndex = nullptr;
    if (mCharStrings != nullptr)
    {
        for (unsigned long i = 0; i < mFontsCount; ++i)
            delete[] mCharStrings[i].mCharStringsIndex;
        delete[] mCharStrings;
        mCharStrings = nullptr;
    }
    delete[] mPrivateDicts;
    mPrivateDicts = nullptr;

    auto itLocalSubrs = mLocalSubrs.begin();
    for (; itLocalSubrs != mLocalSubrs.end(); ++itLocalSubrs)
    {
        delete[] itLocalSubrs->second->mCharStringsIndex;
        delete itLocalSubrs->second;
    }

    auto itCharSets = mCharSets.begin();
    for (; itCharSets != mCharSets.end(); ++itCharSets)
    {
        delete[](*itCharSets)->mSIDs;
        (*itCharSets)->mSIDToGlyphMap.clear();
        delete (*itCharSets);
    }
    mCharSets.clear();

    auto itEncodings = mEncodings.begin();
    for (; itEncodings != mEncodings.end(); ++itEncodings)
    {
        delete[](*itEncodings)->mEncoding;
        delete (*itEncodings);
    }
    mEncodings.clear();
}

void CFFFileInput::Reset()
{
    FreeData();
}

EStatusCode CFFFileInput::ReadCFFFile(charta::IByteReaderWithPosition *inCFFFile)
{
    EStatusCode status;

    do
    {
        FreeData();

        mPrimitivesReader.SetStream(inCFFFile);
        mCFFOffset = inCFFFile->GetCurrentPosition();

        status = ReadHeader();
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile, Failed to read header");
            break;
        }

        // skip to name
        if (mHeader.hdrSize > 4)
            mPrimitivesReader.Skip(mHeader.hdrSize - 4);

        status = ReadNameIndex();
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile Failed to read name");
            break;
        }

        status = ReadTopDictIndex();
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile failed to read top index");
            break;
        }

        status = ReadStringIndex();
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile failed to read top index");
            break;
        }

        status = ReadGlobalSubrs();
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile failed to read global subrs");
            break;
        }

        status = ReadCharStrings();
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile failed to read charstrings");
            break;
        }

        status = ReadPrivateDicts();
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile failed to read charstrings");
            break;
        }

        status = ReadLocalSubrs();
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile failed to read local subrs");
            break;
        }

        status = ReadCharsets();
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile failed to read char set");
            break;
        }

        status = ReadEncodings();
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile failed to read encodings");
            break;
        }

        status = ReadCIDInformation();
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile failed to read CID Information");
            break;
        }

    } while (false);
    return status;
}

EStatusCode CFFFileInput::ReadHeader()
{
    mPrimitivesReader.ReadCard8(mHeader.major);
    mPrimitivesReader.ReadCard8(mHeader.minor);
    mPrimitivesReader.ReadCard8(mHeader.hdrSize);
    mPrimitivesReader.ReadOffSize(mHeader.offSize);

    return mPrimitivesReader.GetInternalState();
}

EStatusCode CFFFileInput::ReadIndexHeader(unsigned long **outOffsets, uint16_t &outItemsCount)
{
    uint8_t offSizeForIndex;

    EStatusCode status = mPrimitivesReader.ReadCard16(outItemsCount);
    if (status != charta::eSuccess)
        return charta::eFailure;

    if (0 == outItemsCount)
    {
        *outOffsets = nullptr;
        return charta::eSuccess;
    }

    mPrimitivesReader.ReadOffSize(offSizeForIndex);
    if (status != charta::eSuccess)
        return charta::eFailure;

    mPrimitivesReader.SetOffSize(offSizeForIndex);
    *outOffsets = new unsigned long[outItemsCount + 1];

    for (unsigned long i = 0; i <= outItemsCount && status == eSuccess; ++i)
        status = mPrimitivesReader.ReadOffset((*outOffsets)[i]);

    if (status != eSuccess)
        return status;
    return mPrimitivesReader.GetInternalState();
}

EStatusCode CFFFileInput::ReadNameIndex()
{
    mNameIndexPosition = mPrimitivesReader.GetCurrentPosition();

    unsigned long *offsets;
    EStatusCode status = ReadIndexHeader(&offsets, mFontsCount);
    uint8_t *buffer;

    do
    {
        if (status != charta::eSuccess)
            break;

        if (offsets[0] != 1)
            mPrimitivesReader.Skip(offsets[0] - 1);

        for (unsigned long i = 0; i < mFontsCount; ++i)
        {
            buffer = new uint8_t[offsets[i + 1] - offsets[i]];
            mPrimitivesReader.Read(buffer, offsets[i + 1] - offsets[i]);
            std::string aName((char *)buffer, offsets[i + 1] - offsets[i]);
            mName.push_back(aName);
            if (buffer[0] != 0) // put in map only valid names
                mNameToIndex.insert(StringToUShort::value_type(aName, i));
            delete[] buffer;
        }

    } while (false);

    delete[] offsets;
    if (status != charta::eSuccess)
        return status;
    return mPrimitivesReader.GetInternalState();
}

BoolAndUShort CFFFileInput::GetIndexForFontName(const std::string &inFontName)
{
    auto it = mNameToIndex.find(inFontName);

    if (it == mNameToIndex.end())
        return BoolAndUShort(false, 0);
    return BoolAndUShort(true, it->second);
}

EStatusCode CFFFileInput::ReadTopDictIndex()
{
    mTopDictIndexPosition = mPrimitivesReader.GetCurrentPosition();

    unsigned long *offsets;
    uint16_t dictionariesCount;
    EStatusCode status = ReadIndexHeader(&offsets, dictionariesCount);

    do
    {
        if (status != charta::eSuccess)
            break;

        if (offsets[0] != 1)
            mPrimitivesReader.Skip(offsets[0] - 1);

        mTopDictIndex = new TopDictInfo[dictionariesCount];

        for (unsigned long i = 0; i < dictionariesCount && (charta::eSuccess == status); ++i)
            status = ReadDict(offsets[i + 1] - offsets[i], mTopDictIndex[i].mTopDict);

    } while (false);

    delete[] offsets;
    if (status != charta::eSuccess)
        return status;
    return mPrimitivesReader.GetInternalState();
}

EStatusCode CFFFileInput::ReadDict(unsigned long inReadAmount, UShortToDictOperandListMap &outDict)
{
    long long dictStartPosition = mPrimitivesReader.GetCurrentPosition();
    DictOperandList operands;
    EStatusCode status = charta::eSuccess;
    uint16_t anOperator;
    DictOperand anOperand;
    uint8_t aBuffer;

    while (charta::eSuccess == status &&
           (mPrimitivesReader.GetCurrentPosition() - dictStartPosition < (long long)inReadAmount))
    {
        status = mPrimitivesReader.ReadByte(aBuffer);
        if (status != charta::eSuccess)
            break;
        if (mPrimitivesReader.IsDictOperator(aBuffer))
        { // operator
            status = mPrimitivesReader.ReadDictOperator(aBuffer, anOperator);
            if (status != charta::eSuccess)
                break;
            outDict.insert(UShortToDictOperandListMap::value_type(anOperator, operands));
            operands.clear();
        }
        else // operand
        {
            status = mPrimitivesReader.ReadDictOperand(aBuffer, anOperand);
            if (status != charta::eSuccess)
                break;
            operands.push_back(anOperand);
        }
    }

    return status;
}

EStatusCode CFFFileInput::ReadStringIndex()
{
    mStringIndexPosition = mPrimitivesReader.GetCurrentPosition();

    unsigned long *offsets;
    EStatusCode status = ReadIndexHeader(&offsets, mStringsCount);

    do
    {
        if (status != charta::eSuccess)
            break;

        if (0 == mStringsCount)
        {
            mStrings = nullptr;
            break;
        }

        if (offsets[0] != 1)
            mPrimitivesReader.Skip(offsets[0] - 1);

        mStrings = new char *[mStringsCount];

        unsigned long i;
        for (i = 0; i < mStringsCount && (charta::eSuccess == status); ++i)
        {
            mStrings[i] = new char[offsets[i + 1] - offsets[i] + 1];
            status = mPrimitivesReader.Read((uint8_t *)mStrings[i], offsets[i + 1] - offsets[i]);
            if (status != charta::eSuccess)
                break;
            mStrings[i][offsets[i + 1] - offsets[i]] = 0;
        }

        // failure case, null all the rest of the strings for later delete to not perofrm errors
        if (status != charta::eSuccess)
        {
            for (; i < mStringsCount; ++i)
                mStrings[i] = nullptr;
        }

        // now create the string to SID map
        for (i = 0; i < N_STD_STRINGS; ++i)
            mStringToSID.insert(CharPToUShortMap::value_type(scStandardStrings[i], i));
        for (; i < N_STD_STRINGS + mStringsCount; ++i)
            mStringToSID.insert(CharPToUShortMap::value_type(mStrings[i - N_STD_STRINGS], i));

    } while (false);

    delete[] offsets;
    if (status != charta::eSuccess)
        return status;
    return mPrimitivesReader.GetInternalState();
}

std::string CFFFileInput::GetStringForSID(uint16_t inSID) const
{
    if (inSID < N_STD_STRINGS)
        return scStandardStrings[inSID];
    if (inSID < N_STD_STRINGS + mStringsCount)
        return mStrings[inSID - N_STD_STRINGS];
    return "";
}

EStatusCode CFFFileInput::ReadGlobalSubrs()
{
    mGlobalSubrsPosition = mPrimitivesReader.GetCurrentPosition();
    mGlobalSubrs.mCharStringsType = 2; // always 2
    return ReadSubrsFromIndex(mGlobalSubrs.mCharStringsCount, &(mGlobalSubrs.mCharStringsIndex));
}

EStatusCode CFFFileInput::ReadSubrsFromIndex(uint16_t &outSubrsCount, CharStringsIndex *outSubrsIndex)
{
    unsigned long *offsets;
    EStatusCode status = ReadIndexHeader(&offsets, outSubrsCount);

    do
    {
        if (status != charta::eSuccess)
            break;

        if (0 == outSubrsCount)
        {
            *outSubrsIndex = nullptr;
            break;
        }

        // just taking a snapshot of positions here
        *outSubrsIndex = new CharString[outSubrsCount];

        long long dataStartPosition = mPrimitivesReader.GetCurrentPosition();

        for (unsigned long i = 0; i < outSubrsCount; ++i)
        {
            (*outSubrsIndex)[i].mStartPosition = dataStartPosition + offsets[i] - 1;
            (*outSubrsIndex)[i].mEndPosition = dataStartPosition + offsets[i + 1] - 1;
            (*outSubrsIndex)[i].mIndex = i;
        }

        // for good faith put the pointer at the end now (if someone wants to take a snapshot)
        mPrimitivesReader.Skip(offsets[outSubrsCount] - 1);
    } while (false);

    delete[] offsets;
    if (status != charta::eSuccess)
        return status;
    return mPrimitivesReader.GetInternalState();
}

EStatusCode CFFFileInput::ReadCharStrings()
{
    // scan all charstrings of all included fonts
    mCharStrings = new CharStrings[mFontsCount];
    EStatusCode status = charta::eSuccess;

    for (unsigned long i = 0; i < mFontsCount && (charta::eSuccess == status); ++i)
    {
        long long charStringsPosition = GetCharStringsPosition(i);
        mCharStrings[i].mCharStringsType = (uint8_t)GetCharStringType(i);
        if (0 == charStringsPosition)
        {
            mCharStrings[i].mCharStringsCount = 0;
            mCharStrings[i].mCharStringsIndex = nullptr;
        }
        else
        {
            mPrimitivesReader.SetOffset(charStringsPosition);
            status = ReadSubrsFromIndex(mCharStrings[i].mCharStringsCount, &(mCharStrings[i].mCharStringsIndex));
        }
    }

    if (status != charta::eSuccess)
        return status;
    return mPrimitivesReader.GetInternalState();
}

static const uint16_t scCharStringsKey = 17;
long long CFFFileInput::GetCharStringsPosition(uint16_t inFontIndex)
{
    return GetSingleIntegerValue(inFontIndex, scCharStringsKey, 0);
}

long CFFFileInput::GetSingleIntegerValue(uint16_t inFontIndex, uint16_t inKey, long inDefault)
{
    return GetSingleIntegerValueFromDict(mTopDictIndex[inFontIndex].mTopDict, inKey, inDefault);
}

long CFFFileInput::GetSingleIntegerValueFromDict(const UShortToDictOperandListMap &inDict, uint16_t inKey,
                                                 long inDefault)
{
    auto it = inDict.find(inKey);

    if (it != inDict.end())
        return it->second.front().IntegerValue;
    return inDefault;
}

static const uint16_t scCharstringType = 0x0C06;
long CFFFileInput::GetCharStringType(uint16_t inFontIndex)
{
    return GetSingleIntegerValue(inFontIndex, scCharstringType, 2);
}

EStatusCode CFFFileInput::ReadPrivateDicts()
{
    mPrivateDicts = new PrivateDictInfo[mFontsCount];
    EStatusCode status = charta::eSuccess;

    for (unsigned long i = 0; i < mFontsCount && (charta::eSuccess == status); ++i)
        status = ReadPrivateDict(mTopDictIndex[i].mTopDict, mPrivateDicts + i);

    if (status != charta::eSuccess)
        return status;
    return mPrimitivesReader.GetInternalState();
}

static const uint16_t scPrivate = 18;
EStatusCode CFFFileInput::ReadPrivateDict(const UShortToDictOperandListMap &inReferencingDict,
                                          PrivateDictInfo *outPrivateDict)
{
    EStatusCode status = charta::eSuccess;
    auto it = inReferencingDict.find(scPrivate);

    outPrivateDict->mLocalSubrs = nullptr;
    if (it == inReferencingDict.end())
    {
        outPrivateDict->mPrivateDictStart = 0;
        outPrivateDict->mPrivateDictEnd = 0;
    }
    else
    {
        outPrivateDict->mPrivateDictStart = (long long)it->second.back().IntegerValue;
        outPrivateDict->mPrivateDictEnd = (long long)(it->second.back().IntegerValue + it->second.front().IntegerValue);

        mPrimitivesReader.SetOffset(it->second.back().IntegerValue);
        status = ReadDict(it->second.front().IntegerValue, outPrivateDict->mPrivateDict);
    }
    return status;
}

EStatusCode CFFFileInput::ReadLocalSubrs()
{
    // scan all subrs of all included fonts
    EStatusCode status = charta::eSuccess;

    for (unsigned long i = 0; i < mFontsCount && (charta::eSuccess == status); ++i)
        status = ReadLocalSubrsForPrivateDict(mPrivateDicts + i, (uint8_t)GetCharStringType(i));

    if (status != charta::eSuccess)
        return status;
    return mPrimitivesReader.GetInternalState();
}

static const uint16_t scSubrs = 19;
EStatusCode CFFFileInput::ReadLocalSubrsForPrivateDict(PrivateDictInfo *inPrivateDict, uint8_t inCharStringType)
{
    EStatusCode status = charta::eSuccess;
    long long subrsPosition = GetSingleIntegerValueFromDict(inPrivateDict->mPrivateDict, scSubrs, 0);

    if (0 == subrsPosition)
    {
        inPrivateDict->mLocalSubrs = nullptr;
    }
    else
    {
        auto it = mLocalSubrs.find(inPrivateDict->mPrivateDictStart + subrsPosition);
        if (it == mLocalSubrs.end())
        {
            auto *charStrings = new CharStrings();
            charStrings->mCharStringsType = inCharStringType;
            mPrimitivesReader.SetOffset(inPrivateDict->mPrivateDictStart + subrsPosition);
            status = ReadSubrsFromIndex(charStrings->mCharStringsCount, &(charStrings->mCharStringsIndex));
            if (status != charta::eSuccess)
                TRACE_LOG("CFFFileInput::ReadLocalSubrs, failed to read local subrs");
            else
                it = mLocalSubrs
                         .insert(LongFilePositionTypeToCharStringsMap::value_type(
                             inPrivateDict->mPrivateDictStart + subrsPosition, charStrings))
                         .first;
        }
        inPrivateDict->mLocalSubrs = it->second;
    }
    return status;
}

using LongFilePositionTypeToCharSetInfoMap = std::map<long long, CharSetInfo *>;

static const uint16_t scROS = 0xC1E;
EStatusCode CFFFileInput::ReadCharsets()
{
    // read all charsets
    EStatusCode status = charta::eSuccess;
    LongFilePositionTypeToCharSetInfoMap offsetToIndex;
    LongFilePositionTypeToCharSetInfoMap::iterator it;

    for (unsigned long i = 0; i < mFontsCount && (charta::eSuccess == status); ++i)
    {
        long long charsetPosition = GetCharsetPosition(i);
        it = offsetToIndex.find(charsetPosition);
        if (it == offsetToIndex.end())
        {
            auto *charSet = new CharSetInfo();
            bool isCID = mTopDictIndex[i].mTopDict.find(scROS) != mTopDictIndex[i].mTopDict.end();
            if (charsetPosition <= 2)
            {
                charSet->mType = (ECharSetType)charsetPosition;
                charSet->mSIDs = nullptr;
                if (!isCID) // collect SID->Glyph map only if not CID, in which case SIDs are CIDs...and what i'm using
                            // the map for is irrelevant
                    SetupSIDToGlyphMapWithStandard(scDefaultCharsets[charsetPosition],
                                                   scDefaultCharsetsSizes[charsetPosition], charSet->mSIDToGlyphMap,
                                                   mCharStrings[i]);
            }
            else
            {
                uint8_t charsetFormat;
                charSet->mType = eCharSetCustom;
                mPrimitivesReader.SetOffset(charsetPosition);
                mPrimitivesReader.ReadCard8(charsetFormat);

                if (0 == charsetFormat)
                    status = ReadFormat0Charset(isCID, charSet->mSIDToGlyphMap, &charSet->mSIDs, mCharStrings[i]);
                else if (1 == charsetFormat)
                    status = ReadFormat1Charset(isCID, charSet->mSIDToGlyphMap, &charSet->mSIDs, mCharStrings[i]);
                else // 2 charset format
                    status = ReadFormat2Charset(isCID, charSet->mSIDToGlyphMap, &charSet->mSIDs, mCharStrings[i]);
            }
            mCharSets.push_back(charSet);
            it = offsetToIndex.insert(LongFilePositionTypeToCharSetInfoMap::value_type(charsetPosition, charSet)).first;
        }
        mTopDictIndex[i].mCharSet = it->second;
    }

    if (status != charta::eSuccess)
        return status;
    return mPrimitivesReader.GetInternalState();
}

using LongFilePositionTypeToEncodingsInfoMap = std::map<long long, EncodingsInfo *>;

EStatusCode CFFFileInput::ReadEncodings()
{
    // read all encodings positions
    EStatusCode status = charta::eSuccess;
    LongFilePositionTypeToEncodingsInfoMap offsetToEncoding;
    LongFilePositionTypeToEncodingsInfoMap::iterator it;

    for (unsigned long i = 0; i < mFontsCount && (charta::eSuccess == status); ++i)
    {
        long long encodingPosition = GetEncodingPosition(i);
        it = offsetToEncoding.find(encodingPosition);
        if (it == offsetToEncoding.end())
        {
            auto *encoding = new EncodingsInfo();
            ReadEncoding(encoding, encodingPosition);
            mEncodings.push_back(encoding);
            it = offsetToEncoding.insert(LongFilePositionTypeToEncodingsInfoMap::value_type(encodingPosition, encoding))
                     .first;
        }
        mTopDictIndex[i].mEncoding = it->second;
    }

    if (status != charta::eSuccess)
        return status;
    return mPrimitivesReader.GetInternalState();
}

void CFFFileInput::ReadEncoding(EncodingsInfo *inEncoding, long long inEncodingPosition)
{
    if (inEncodingPosition <= 1)
    {
        inEncoding->mEncodingStart = inEncoding->mEncodingEnd = inEncodingPosition;
        inEncoding->mType = (EEncodingType)inEncodingPosition;
    }
    else
    {
        inEncoding->mType = eEncodingCustom;
        uint8_t encodingFormat = 0;
        inEncoding->mEncodingStart = inEncodingPosition;
        mPrimitivesReader.SetOffset(inEncodingPosition);
        mPrimitivesReader.ReadCard8(encodingFormat);

        if (0 == (encodingFormat & 0x1))
        {
            mPrimitivesReader.ReadCard8(inEncoding->mEncodingsCount);
            if (inEncoding->mEncodingsCount > 0)
            {
                inEncoding->mEncoding = new uint8_t[inEncoding->mEncodingsCount];
                for (uint8_t i = 0; i < inEncoding->mEncodingsCount; ++i)
                    mPrimitivesReader.ReadCard8(inEncoding->mEncoding[i]);
            }
        }
        else // format = 1
        {
            uint8_t rangesCount = 0;
            mPrimitivesReader.ReadCard8(rangesCount);
            if (rangesCount > 0)
            {
                uint8_t firstCode;
                uint8_t left;

                inEncoding->mEncodingsCount = 0;
                // get the encoding count (yap, reading twice here)
                for (uint8_t i = 0; i < rangesCount; ++i)
                {
                    mPrimitivesReader.ReadCard8(firstCode);
                    mPrimitivesReader.ReadCard8(left);
                    inEncoding->mEncodingsCount += left;
                }
                inEncoding->mEncoding = new uint8_t[inEncoding->mEncodingsCount];
                mPrimitivesReader.SetOffset(inEncodingPosition + 2); // reset encoding to beginning of range reading

                // now read the encoding array
                uint8_t encodingIndex = 0;
                for (uint8_t i = 0; i < rangesCount; ++i)
                {
                    mPrimitivesReader.ReadCard8(firstCode);
                    mPrimitivesReader.ReadCard8(left);
                    for (uint8_t j = 0; j < left; ++j)
                        inEncoding->mEncoding[encodingIndex + j] = firstCode + j;
                    encodingIndex += left;
                }
            }
        }
        if ((encodingFormat & 0x80) != 0) // supplaments exist, need to add to encoding end
        {
            mPrimitivesReader.SetOffset(inEncoding->mEncodingEnd); // set position to end of encoding, and start of
                                                                   // supplamental, so that can read their count
            uint8_t supplamentalsCount = 0;
            mPrimitivesReader.ReadCard8(supplamentalsCount);
            if (supplamentalsCount > 0)
            {
                uint8_t encoding;
                uint16_t SID;
                for (uint8_t i = 0; i < supplamentalsCount; ++i)
                {
                    mPrimitivesReader.ReadCard8(encoding);
                    mPrimitivesReader.ReadCard16(SID);

                    auto it = inEncoding->mSupplements.find(SID);
                    if (it == inEncoding->mSupplements.end())
                        it = inEncoding->mSupplements.insert(UShortToByteList::value_type(SID, ByteList())).first;
                    it->second.push_back(encoding);
                }
            }
        }
        inEncoding->mEncodingEnd = mPrimitivesReader.GetCurrentPosition();
    }
}

void CFFFileInput::SetupSIDToGlyphMapWithStandard(const uint16_t *inStandardCharSet, uint16_t inStandardCharSetLength,
                                                  UShortToCharStringMap &ioCharMap, const CharStrings &inCharStrings)
{
    ioCharMap.insert(UShortToCharStringMap::value_type(0, inCharStrings.mCharStringsIndex));
    uint16_t i;
    for (i = 1; i < inCharStrings.mCharStringsCount && i < inStandardCharSetLength; ++i)
    {
        ioCharMap.insert(UShortToCharStringMap::value_type(inStandardCharSet[i], inCharStrings.mCharStringsIndex + i));
    }
}

EStatusCode CFFFileInput::ReadFormat0Charset(bool inIsCID, UShortToCharStringMap &ioGlyphMap, uint16_t **inSIDArray,
                                             const CharStrings &inCharStrings)
{
    // for CIDs don't bother filling up the SID->glyph map. it ain't SIDs
    if (!inIsCID)
        ioGlyphMap.insert(UShortToCharStringMap::value_type(0, inCharStrings.mCharStringsIndex));
    *inSIDArray = new uint16_t[inCharStrings.mCharStringsCount];
    (*inSIDArray)[0] = 0;

    if (inIsCID)
    {
        for (uint16_t i = 1; i < inCharStrings.mCharStringsCount; ++i)
            mPrimitivesReader.ReadSID((*inSIDArray)[i]);
    }
    else
    {
        for (uint16_t i = 1; i < inCharStrings.mCharStringsCount; ++i)
        {
            uint16_t sid;
            mPrimitivesReader.ReadSID(sid);
            (*inSIDArray)[i] = sid;

            ioGlyphMap.insert(UShortToCharStringMap::value_type(sid, inCharStrings.mCharStringsIndex + i));
        }
    }
    return mPrimitivesReader.GetInternalState();
}

EStatusCode CFFFileInput::ReadFormat1Charset(bool inIsCID, UShortToCharStringMap &ioGlyphMap, uint16_t **inSIDArray,
                                             const CharStrings &inCharStrings)
{
    if (!inIsCID)
        ioGlyphMap.insert(UShortToCharStringMap::value_type(0, inCharStrings.mCharStringsIndex));
    *inSIDArray = new uint16_t[inCharStrings.mCharStringsCount];
    (*inSIDArray)[0] = 0;
    unsigned long glyphIndex = 1;
    uint16_t sid;
    uint8_t left;

    if (inIsCID)
    {
        while (glyphIndex < inCharStrings.mCharStringsCount)
        {
            mPrimitivesReader.ReadSID(sid);
            mPrimitivesReader.ReadCard8(left);
            for (unsigned long i = 0; i <= left && glyphIndex < inCharStrings.mCharStringsCount; ++i, ++glyphIndex)
                (*inSIDArray)[glyphIndex] = sid + i;
        }
    }
    else
    {
        while (glyphIndex < inCharStrings.mCharStringsCount)
        {
            mPrimitivesReader.ReadSID(sid);
            mPrimitivesReader.ReadCard8(left);
            for (unsigned long i = 0; i <= left && glyphIndex < inCharStrings.mCharStringsCount; ++i, ++glyphIndex)
            {
                ioGlyphMap.insert(
                    UShortToCharStringMap::value_type(sid + i, inCharStrings.mCharStringsIndex + glyphIndex));
                (*inSIDArray)[glyphIndex] = sid + i;
            }
        }
    }
    return mPrimitivesReader.GetInternalState();
}

EStatusCode CFFFileInput::ReadFormat2Charset(bool inIsCID, UShortToCharStringMap &ioGlyphMap, uint16_t **inSIDArray,
                                             const CharStrings &inCharStrings)
{
    if (!inIsCID)
        ioGlyphMap.insert(UShortToCharStringMap::value_type(0, inCharStrings.mCharStringsIndex));
    *inSIDArray = new uint16_t[inCharStrings.mCharStringsCount];
    (*inSIDArray)[0] = 0;
    uint16_t glyphIndex = 1;
    uint16_t sid;
    uint16_t left;

    if (inIsCID)
    {
        while (glyphIndex < inCharStrings.mCharStringsCount)
        {
            mPrimitivesReader.ReadSID(sid);
            mPrimitivesReader.ReadCard16(left);
            for (unsigned long i = 0; i <= left && glyphIndex < inCharStrings.mCharStringsCount; ++i, ++glyphIndex)
                (*inSIDArray)[glyphIndex] = sid + i;
        }
    }
    else
    {
        while (glyphIndex < inCharStrings.mCharStringsCount)
        {
            mPrimitivesReader.ReadSID(sid);
            mPrimitivesReader.ReadCard16(left);
            for (unsigned long i = 0; i <= left && glyphIndex < inCharStrings.mCharStringsCount; ++i, ++glyphIndex)
            {
                ioGlyphMap.insert(
                    UShortToCharStringMap::value_type(sid + i, inCharStrings.mCharStringsIndex + glyphIndex));
                (*inSIDArray)[glyphIndex] = sid + i;
            }
        }
    }
    return mPrimitivesReader.GetInternalState();
}

static const uint16_t scCharset = 15;
long long CFFFileInput::GetCharsetPosition(uint16_t inFontIndex)
{
    return (long long)GetSingleIntegerValue(inFontIndex, scCharset, 0);
}

static const uint16_t scEncoding = 16;
long long CFFFileInput::GetEncodingPosition(uint16_t inFontIndex)
{
    return (long long)GetSingleIntegerValue(inFontIndex, scEncoding, 0);
}

EStatusCode CFFFileInput::CalculateDependenciesForCharIndex(uint16_t inFontIndex, uint16_t inCharStringIndex,
                                                            CharString2Dependencies &ioDependenciesInfo)
{
    CharStringType2Interpreter interpreter;

    EStatusCode status = PrepareForGlyphIntepretation(inFontIndex, inCharStringIndex);
    if (status != charta::eFailure)
    {
        mCurrentDependencies = &ioDependenciesInfo;
        return interpreter.Intepret(*GetGlyphCharString(inFontIndex, inCharStringIndex), this);
    }
    return status;
}

EStatusCode CFFFileInput::PrepareForGlyphIntepretation(uint16_t inFontIndex, uint16_t inCharStringIndex)
{
    if (inFontIndex >= mFontsCount)
    {
        TRACE_LOG2("CFFFileInput::PrepareForGlyphIntepretation, inFontIndex = %d is invalid. there are %d fonts in the "
                   "CFF segment",
                   inFontIndex, mFontsCount);
        return charta::eFailure;
    }

    if (mCharStrings[inFontIndex].mCharStringsCount <= inCharStringIndex)
    {
        TRACE_LOG2("CFFFileInput::PrepareForGlyphIntepretation, inCharStringIndex = %d is invalid. there are %d "
                   "charsringd in the CFF segment for the requested font",
                   inCharStringIndex, mCharStrings[inFontIndex].mCharStringsCount);
        return charta::eFailure;
    }

    if (2 == mCharStrings[inFontIndex].mCharStringsType)
    {
        if (mTopDictIndex[inFontIndex].mFDSelect != nullptr) // CIDs have FDSelect
        {
            mCurrentLocalSubrs = mTopDictIndex[inFontIndex].mFDSelect[inCharStringIndex]->mPrivateDict.mLocalSubrs;
            mCurrentCharsetInfo = mTopDictIndex[inFontIndex].mCharSet;
            mCurrentDependencies = nullptr;
        }
        else
        {
            mCurrentLocalSubrs = mPrivateDicts[inFontIndex].mLocalSubrs;
            mCurrentCharsetInfo = mTopDictIndex[inFontIndex].mCharSet;
            mCurrentDependencies = nullptr;
        }
        return charta::eSuccess;
    }

    TRACE_LOG1("CFFFileInput::PrepareForGlyphIntepretation, unsupported charstring format = %d. only type 2 "
               "charstrings are supported",
               mCharStrings[inFontIndex].mCharStringsType);
    return charta::eFailure;
}

CharString *CFFFileInput::GetGlyphCharString(uint16_t inFontIndex, uint16_t inCharStringIndex)
{
    if (inFontIndex >= mFontsCount)
    {
        TRACE_LOG2("CFFFileInput::CalculateDependenciesForCharIndex, inFontIndex = %d is invalid. there are %d fonts "
                   "in the CFF segment",
                   inFontIndex, mFontsCount);
        return nullptr;
    }

    if (mCharStrings[inFontIndex].mCharStringsCount <= inCharStringIndex)
    {
        TRACE_LOG2("CFFFileInput::CalculateDependenciesForCharIndex, inCharStringIndex = %d is invalid. there are %d "
                   "charsringd in the CFF segment for the requested font",
                   inCharStringIndex, mCharStrings[inFontIndex].mCharStringsCount);
        return nullptr;
    }

    return mCharStrings[inFontIndex].mCharStringsIndex + inCharStringIndex;
}

EStatusCode CFFFileInput::ReadCharString(long long inCharStringStart, long long inCharStringEnd,
                                         uint8_t **outCharString)
{
    EStatusCode status = charta::eSuccess;
    mPrimitivesReader.SetOffset(inCharStringStart);
    *outCharString = nullptr;

    do
    {
        *outCharString = new uint8_t[(size_t)(inCharStringEnd - inCharStringStart)];

        status = mPrimitivesReader.Read(*outCharString, (size_t)(inCharStringEnd - inCharStringStart));
        if (status != charta::eSuccess)
            break;

    } while (false);

    if (status != charta::eSuccess && (*outCharString != nullptr))
        delete[] * outCharString;

    return status;
}

CharString *CFFFileInput::GetLocalSubr(long inSubrIndex)
{
    // locate local subr and return. also - push it to the dependendecy stack to start calculating dependencies for it
    // also - record dependency on this subr.
    uint16_t biasedIndex = GetBiasedIndex(mCurrentLocalSubrs->mCharStringsCount, inSubrIndex);

    if (biasedIndex < mCurrentLocalSubrs->mCharStringsCount)
    {
        CharString *returnValue = mCurrentLocalSubrs->mCharStringsIndex + biasedIndex;
        if (mCurrentDependencies != nullptr)
            mCurrentDependencies->mLocalSubrs.insert(biasedIndex);
        return returnValue;
    }
    return nullptr;
}

uint16_t CFFFileInput::GetBiasedIndex(uint16_t inSubroutineCollectionSize, long inSubroutineIndex)
{
    if (inSubroutineCollectionSize < 1240)
        return (uint16_t)(107 + inSubroutineIndex);
    if (inSubroutineCollectionSize < 33900)
        return (uint16_t)(1131 + inSubroutineIndex);
    return (uint16_t)(32768 + inSubroutineIndex);
}

CharString *CFFFileInput::GetGlobalSubr(long inSubrIndex)
{
    uint16_t biasedIndex = GetBiasedIndex(mGlobalSubrs.mCharStringsCount, inSubrIndex);

    if (biasedIndex < mGlobalSubrs.mCharStringsCount)
    {
        CharString *returnValue = mGlobalSubrs.mCharStringsIndex + biasedIndex;
        if (mCurrentDependencies != nullptr)
            mCurrentDependencies->mGlobalSubrs.insert(biasedIndex);
        return returnValue;
    }
    return nullptr;
}

EStatusCode CFFFileInput::Type2Endchar(const CharStringOperandList &inOperandList)
{
    // i'm using EndChar here to check the depracated usage, which creates
    // dependency on another charachter. as for finalizing the intepretation, i don't
    // need to do anything here, and just make sure to pop the whole dependency stack
    // in the interpretation calling method.

    if (inOperandList.size() >= 4) // meaning it's got the depracated seac usage. 2 topmost charachters on the stack are
                                   // charachter codes of off StandardEncoding
    {
        auto it = inOperandList.rbegin();
        uint8_t characterCode1, characterCode2;

        characterCode1 = it->IsInteger ? (uint8_t)it->IntegerValue : (uint8_t)it->RealValue;
        ++it;
        characterCode2 = it->IsInteger ? (uint8_t)it->IntegerValue : (uint8_t)it->RealValue;

        CharString *character1 = GetCharacterFromStandardEncoding(characterCode1);
        CharString *character2 = GetCharacterFromStandardEncoding(characterCode2);

        if ((character1 != nullptr) && (character2 != nullptr) && (mCurrentDependencies != nullptr))
        {
            mCurrentDependencies->mCharCodes.insert(character1->mIndex);
            mCurrentDependencies->mCharCodes.insert(character2->mIndex);
            return charta::eSuccess;
        }
        return charta::eFailure;
    }

    return charta::eSuccess;
}

CharString *CFFFileInput::GetCharacterFromStandardEncoding(uint8_t inCharacterCode)
{
    StandardEncoding standardEncoding;
    const char *glyphName = standardEncoding.GetEncodedGlyphName(inCharacterCode);
    auto itStringToSID = mStringToSID.find(glyphName);

    if (itStringToSID != mStringToSID.end())
    {
        auto itSIDToGlyph = mCurrentCharsetInfo->mSIDToGlyphMap.find(itStringToSID->second);
        if (itSIDToGlyph != mCurrentCharsetInfo->mSIDToGlyphMap.end())
            return itSIDToGlyph->second;
        return nullptr;
    }
    return nullptr;
}

EStatusCode CFFFileInput::ReadCIDInformation()
{
    EStatusCode status = charta::eSuccess;

    for (unsigned long i = 0; i < mFontsCount && (charta::eSuccess == status); ++i)
    {
        // CID font will be identified by the existance of the ROS entry
        if (mTopDictIndex[i].mTopDict.find(scROS) != mTopDictIndex[i].mTopDict.end())
        {
            status = ReadFDArray(i);
            if (status != charta::eSuccess)
            {
                TRACE_LOG1("CFFFileInput::ReadCIDInformation, unable to read FDArray for font index %d", i);
                break;
            }

            status = ReadFDSelect(i);
            if (status != charta::eSuccess)
            {
                TRACE_LOG1("CFFFileInput::ReadCIDInformation, unable to read FDSelect for font index %d", i);
                break;
            }
        }
    }

    return status;
}

EStatusCode CFFFileInput::ReadFDArray(uint16_t inFontIndex)
{
    long long fdArrayLocation = GetFDArrayPosition(inFontIndex);

    // supposed to get here only for CIDs. and they must have an FDArray...so if it doesn't - fail
    if (0 == fdArrayLocation)
        return charta::eFailure;

    mPrimitivesReader.SetOffset(fdArrayLocation);

    unsigned long *offsets;
    uint16_t dictionariesCount;
    unsigned long i;
    EStatusCode status = ReadIndexHeader(&offsets, dictionariesCount);

    do
    {
        if (status != charta::eSuccess)
            break;

        if (offsets[0] != 1)
            mPrimitivesReader.Skip(offsets[0] - 1);

        mTopDictIndex[inFontIndex].mFDArray = new FontDictInfo[dictionariesCount];

        for (i = 0; i < dictionariesCount && (charta::eSuccess == status); ++i)
        {
            mTopDictIndex[inFontIndex].mFDArray[i].mFontDictStart = mPrimitivesReader.GetCurrentPosition();
            status = ReadDict(offsets[i + 1] - offsets[i], mTopDictIndex[inFontIndex].mFDArray[i].mFontDict);
            if (status != charta::eSuccess)
            {
                TRACE_LOG("CFFFileInput::ReadFDArray, failed to read FDArray");
                break;
            }
            mTopDictIndex[inFontIndex].mFDArray[i].mFontDictEnd = mPrimitivesReader.GetCurrentPosition();
        }

        // another loop for reading the privates [should be one per font dict]. make sure to get their font subrs
        // reference right
        for (i = 0; i < dictionariesCount && (charta::eSuccess == status); ++i)
        {
            status = ReadPrivateDict(mTopDictIndex[inFontIndex].mFDArray[i].mFontDict,
                                     &(mTopDictIndex[inFontIndex].mFDArray[i].mPrivateDict));
            if (charta::eSuccess == status)
                status = ReadLocalSubrsForPrivateDict(&(mTopDictIndex[inFontIndex].mFDArray[i].mPrivateDict),
                                                      (uint8_t)GetCharStringType(inFontIndex));
        }

    } while (false);

    delete[] offsets;
    if (status != charta::eSuccess)
        return status;
    return mPrimitivesReader.GetInternalState();
}

static const uint16_t scFDArray = 0xC24;
long long CFFFileInput::GetFDArrayPosition(uint16_t inFontIndex)
{
    return GetSingleIntegerValue(inFontIndex, scFDArray, 0);
}

EStatusCode CFFFileInput::ReadFDSelect(uint16_t inFontIndex)
{
    long long fdSelectLocation = GetFDSelectPosition(inFontIndex);
    uint16_t glyphCount = mCharStrings[inFontIndex].mCharStringsCount;
    EStatusCode status = charta::eSuccess;
    uint8_t format;

    // supposed to get here only for CIDs. and they must have an FDSelect...so if it doesn't - fail
    if (0 == fdSelectLocation)
        return charta::eFailure;

    mTopDictIndex[inFontIndex].mFDSelect = new FontDictInfo *[glyphCount];
    mPrimitivesReader.SetOffset(fdSelectLocation);

    mPrimitivesReader.ReadCard8(format);
    if (0 == format)
    {
        uint8_t fdIndex;

        for (unsigned long i = 0; i < glyphCount && charta::eSuccess == status; ++i)
        {
            status = mPrimitivesReader.ReadCard8(fdIndex);
            if (status != charta::eFailure)
                mTopDictIndex[inFontIndex].mFDSelect[i] = mTopDictIndex[inFontIndex].mFDArray + fdIndex;
        }
    }
    else // format 3
    {
        uint16_t rangesCount;
        uint16_t firstGlyphIndex;
        uint16_t nextRangeGlyphIndex;
        uint8_t fdIndex;

        status = mPrimitivesReader.ReadCard16(rangesCount);
        if (status != charta::eFailure)
        {
            status = mPrimitivesReader.ReadCard16(firstGlyphIndex);
            for (unsigned long i = 0; i < rangesCount && charta::eSuccess == status; ++i)
            {
                mPrimitivesReader.ReadCard8(fdIndex);
                mPrimitivesReader.ReadCard16(nextRangeGlyphIndex);
                status = mPrimitivesReader.GetInternalState();
                if (status != charta::eFailure)
                    for (uint16_t j = firstGlyphIndex; j < nextRangeGlyphIndex; ++j)
                        mTopDictIndex[inFontIndex].mFDSelect[j] = mTopDictIndex[inFontIndex].mFDArray + fdIndex;
                firstGlyphIndex = nextRangeGlyphIndex;
            }
        }
    }

    if (status != charta::eSuccess)
        return status;
    return mPrimitivesReader.GetInternalState();
}

static const uint16_t scFDSelect = 0xC25;
long long CFFFileInput::GetFDSelectPosition(uint16_t inFontIndex)
{
    return GetSingleIntegerValue(inFontIndex, scFDSelect, 0);
}

EStatusCode CFFFileInput::ReadCFFFileByIndexOrName(charta::IByteReaderWithPosition *inCFFFile,
                                                   const std::string &inFontName, uint16_t inFontIndex)
{
    // read either by font name or index. read by font name if not empty, otherwise by index
    EStatusCode status;
    uint16_t fontIndex;

    do
    {
        FreeData();
        mPrimitivesReader.SetStream(inCFFFile);
        mCFFOffset = inCFFFile->GetCurrentPosition();

        status = ReadHeader();
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile, Failed to read header");
            break;
        }

        // skip to name
        if (mHeader.hdrSize > 4)
            mPrimitivesReader.Skip(mHeader.hdrSize - 4);

        status = ReadNameIndex();
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile Failed to read name");
            break;
        }

        if (!inFontName.empty())
        {
            BoolAndUShort fontIndexResult = GetIndexForFontName(inFontName);
            if (fontIndexResult.first)
            {
                fontIndex = fontIndexResult.second;
            }
            else
            {
                TRACE_LOG1("CFFFileInput::ReadCFFFile, font name %s was not found in font stream", inFontName.c_str());
                break;
            }
        }
        else
        {
            // make sure index is available in the fontsRange
            if (inFontIndex >= mFontsCount)
            {
                TRACE_LOG2("CFFFileInput::ReadCFFFile, input index (%d) is larger than the maximum possible index (%d)",
                           inFontIndex, mFontsCount - 1);
                break;
            }
            fontIndex = inFontIndex;
        }

        status = ReadTopDictIndex(fontIndex);
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile failed to read top index");
            break;
        }

        status = ReadStringIndex();
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile failed to read top index");
            break;
        }

        status = ReadGlobalSubrs();
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile failed to read global subrs");
            break;
        }

        status = ReadCharStrings(fontIndex);
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile failed to read charstrings");
            break;
        }

        status = ReadPrivateDicts(fontIndex);
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile failed to read charstrings");
            break;
        }

        status = ReadLocalSubrs(fontIndex);
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile failed to read local subrs");
            break;
        }

        status = ReadCharsets(fontIndex);
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile failed to read char set");
            break;
        }

        status = ReadEncodings(fontIndex);
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile failed to read encodings");
            break;
        }

        status = ReadCIDInformation();
        if (status != charta::eSuccess)
        {
            TRACE_LOG("CFFFileInput::ReadCFFFile failed to read CID Information");
            break;
        }

    } while (false);
    return status;
}

EStatusCode CFFFileInput::ReadTopDictIndex(uint16_t inFontIndex)
{
    mTopDictIndexPosition = mPrimitivesReader.GetCurrentPosition();

    // will read only what's necessary for the font index, nulling items in the path

    unsigned long *offsets;
    uint16_t dictionariesCount;
    EStatusCode status = ReadIndexHeader(&offsets, dictionariesCount);

    do
    {
        if (status != charta::eSuccess)
            break;

        // allocate all, but read just the required font
        mTopDictIndex = new TopDictInfo[dictionariesCount];

        if (offsets[inFontIndex] != 1)
            mPrimitivesReader.Skip(offsets[inFontIndex] - 1);
        status = ReadDict(offsets[inFontIndex + 1] - offsets[inFontIndex], mTopDictIndex[inFontIndex].mTopDict);
    } while (false);

    delete[] offsets;
    if (status != charta::eSuccess)
        return status;
    return mPrimitivesReader.GetInternalState();
}

EStatusCode CFFFileInput::ReadCharStrings(uint16_t inFontIndex)
{
    // allocate all, but read just the required one
    mCharStrings = new CharStrings[mFontsCount];
    EStatusCode status = charta::eSuccess;

    long long charStringsPosition = GetCharStringsPosition(inFontIndex);
    mCharStrings[inFontIndex].mCharStringsType = (uint8_t)GetCharStringType(inFontIndex);
    if (charStringsPosition != 0)
    {
        mPrimitivesReader.SetOffset(charStringsPosition);
        status = ReadSubrsFromIndex(mCharStrings[inFontIndex].mCharStringsCount,
                                    &(mCharStrings[inFontIndex].mCharStringsIndex));
    }

    if (status != charta::eSuccess)
        return status;
    return mPrimitivesReader.GetInternalState();
}

EStatusCode CFFFileInput::ReadPrivateDicts(uint16_t inFontIndex)
{
    // allocate all (to maintain indexing...whatever), but read just the relevant font dict
    mPrivateDicts = new PrivateDictInfo[mFontsCount];
    EStatusCode status = ReadPrivateDict(mTopDictIndex[inFontIndex].mTopDict, mPrivateDicts + inFontIndex);

    if (status != charta::eSuccess)
        return status;
    return mPrimitivesReader.GetInternalState();
}

EStatusCode CFFFileInput::ReadLocalSubrs(uint16_t inFontIndex)
{
    // read just the localsubrs of the font private dict
    return ReadLocalSubrsForPrivateDict(mPrivateDicts + inFontIndex, (uint8_t)GetCharStringType(inFontIndex));
}

EStatusCode CFFFileInput::ReadCharsets(uint16_t inFontIndex)
{
    // read all charsets
    EStatusCode status = charta::eSuccess;

    long long charsetPosition = GetCharsetPosition(inFontIndex);
    auto *charSet = new CharSetInfo();
    bool isCID = mTopDictIndex[inFontIndex].mTopDict.find(scROS) != mTopDictIndex[inFontIndex].mTopDict.end();
    if (charsetPosition <= 2)
    {
        charSet->mType = (ECharSetType)charsetPosition;
        charSet->mSIDs = nullptr;
        if (!isCID) // collect SID->Glyph map only if not CID, in which case SIDs are CIDs...and what i'm using the map
                    // for is irrelevant
            SetupSIDToGlyphMapWithStandard(scDefaultCharsets[charsetPosition], scDefaultCharsetsSizes[charsetPosition],
                                           charSet->mSIDToGlyphMap, mCharStrings[inFontIndex]);
    }
    else
    {
        uint8_t charsetFormat;
        charSet->mType = eCharSetCustom;
        mPrimitivesReader.SetOffset(charsetPosition);
        mPrimitivesReader.ReadCard8(charsetFormat);

        if (0 == charsetFormat)
            status = ReadFormat0Charset(isCID, charSet->mSIDToGlyphMap, &charSet->mSIDs, mCharStrings[inFontIndex]);
        else if (1 == charsetFormat)
            status = ReadFormat1Charset(isCID, charSet->mSIDToGlyphMap, &charSet->mSIDs, mCharStrings[inFontIndex]);
        else // 2 charset format
            status = ReadFormat2Charset(isCID, charSet->mSIDToGlyphMap, &charSet->mSIDs, mCharStrings[inFontIndex]);
    }
    mCharSets.push_back(charSet);
    mTopDictIndex[inFontIndex].mCharSet = charSet;

    if (status != charta::eSuccess)
        return status;
    return mPrimitivesReader.GetInternalState();
}

EStatusCode CFFFileInput::ReadEncodings(uint16_t inFontIndex)
{
    // read all encodings positions
    long long encodingPosition = GetEncodingPosition(inFontIndex);
    auto *encoding = new EncodingsInfo();

    ReadEncoding(encoding, encodingPosition);
    mEncodings.push_back(encoding);
    mTopDictIndex[inFontIndex].mEncoding = encoding;

    return mPrimitivesReader.GetInternalState();
}

EStatusCode CFFFileInput::ReadCIDInformation(uint16_t inFontIndex)
{
    EStatusCode status = charta::eSuccess;

    // CID font will be identified by the existance of the ROS entry
    if (mTopDictIndex[inFontIndex].mTopDict.find(scROS) != mTopDictIndex[inFontIndex].mTopDict.end())
    {
        do
        {
            status = ReadFDArray(inFontIndex);
            if (status != charta::eSuccess)
            {
                TRACE_LOG1("CFFFileInput::ReadCIDInformation, unable to read FDArray for font index %d", inFontIndex);
                break;
            }

            status = ReadFDSelect(inFontIndex);
            if (status != charta::eSuccess)
            {
                TRACE_LOG1("CFFFileInput::ReadCIDInformation, unable to read FDSelect for font index %d", inFontIndex);
                break;
            }
        } while (false);
    }
    return status;
}

EStatusCode CFFFileInput::ReadCFFFile(charta::IByteReaderWithPosition *inCFFFile, uint16_t inFontIndex)
{
    return ReadCFFFileByIndexOrName(inCFFFile, "", inFontIndex);
}

EStatusCode CFFFileInput::ReadCFFFile(charta::IByteReaderWithPosition *inCFFFile, const std::string &inFontName)
{
    return ReadCFFFileByIndexOrName(inCFFFile, inFontName, 0);
}

uint16_t CFFFileInput::GetFontsCount(uint16_t /*inFontIndex*/) const
{
    return mFontsCount;
}

uint16_t CFFFileInput::GetCharStringsCount(uint16_t inFontIndex)
{
    if (inFontIndex >= mFontsCount)
        return 0;
    return mCharStrings[inFontIndex].mCharStringsCount;
}

std::string CFFFileInput::GetGlyphName(uint16_t inFontIndex, uint16_t inGlyphIndex)
{
    return GetStringForSID(GetGlyphSID(inFontIndex, inGlyphIndex));
}

uint16_t CFFFileInput::GetGlyphSID(uint16_t inFontIndex, uint16_t inGlyphIndex)
{
    if (inFontIndex >= mFontsCount || inGlyphIndex >= mCharStrings[inFontIndex].mCharStringsCount)
    {
        return 0;
    }

    uint16_t sid;
    if (0 == inGlyphIndex)
    {
        sid = 0;
    }
    else
    {
        if (eCharSetCustom == mTopDictIndex[inFontIndex].mCharSet->mType)
        {
            sid = mTopDictIndex[inFontIndex].mCharSet->mSIDs[inGlyphIndex];
        }
        else
        {
            // SID 0 is omitted for the default charsets
            sid = scDefaultCharsets[(uint8_t)mTopDictIndex[inFontIndex].mCharSet->mType][inGlyphIndex - 1];
        }
    }
    return sid;
}
