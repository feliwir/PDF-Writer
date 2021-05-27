/*
   Source File : PDFParser.h


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
#include "ObjectsBasicTypes.h"
#include "PDFObjectParser.h"
#include "PDFParsingOptions.h"
#include "encryption/DecryptionHelper.h"
#include "io/AdapterIByteReaderWithPositionToIReadPositionProvider.h"
#include "io/IByteReaderWithPosition.h"
#include "objects/PDFDictionary.h"
#include <stdint.h>
#include <stdio.h>

#include <map>
#include <memory>
#include <utility>

class PDFArray;
class PDFStreamInput;
class PDFDictionary;
class PDFName;
class IPDFParserExtender;

typedef std::pair<charta::EStatusCode, charta::IByteReader *> EStatusCodeAndIByteReader;

#define LINE_BUFFER_SIZE 1024

enum EXrefEntryType
{
    eXrefEntryExisting,
    eXrefEntryDelete,
    eXrefEntryStreamObject,
    eXrefEntryUndefined
};

struct XrefEntryInput
{
    XrefEntryInput()
    {
        mObjectPosition = 0;
        mRivision = 0;
        mType = eXrefEntryUndefined;
    }

    // well...it's more like...the first number in a pair on an xref, and the second one. the names
    // are true only for "n" type of entries
    long long mObjectPosition;
    unsigned long mRivision;
    EXrefEntryType mType;
};

struct ObjectStreamHeaderEntry
{
    ObjectIDType mObjectNumber;
    long long mObjectOffset;
};

typedef std::map<ObjectIDType, ObjectStreamHeaderEntry *> ObjectIDTypeToObjectStreamHeaderEntryMap;

class PDFParser
{
  public:
    PDFParser(void);
    virtual ~PDFParser(void);

    // sets the stream to parse, then parses for enough information to be able
    // to parse objects later
    charta::EStatusCode StartPDFParsing(
        charta::IByteReaderWithPosition *inSourceStream,
        const PDFParsingOptions &inOptions = PDFParsingOptions::DefaultPDFParsingOptions());

    // get a parser that can parse objects
    PDFObjectParser &GetObjectParser();

    // get decryption helper - useful to decrypt streams if not using standard operation
    DecryptionHelper &GetDecryptionHelper();

    // below become available after initial parsing [this level is from the header]
    double GetPDFLevel() const;

    // GetTrailer, not calling AddRef
    std::shared_ptr<PDFDictionary> GetTrailer();

    // IMPORTANT! All non "Get" prefix methods below return an object after calling AddRef (or at least make sure
    // reference is added) to handle refcount use the RefCountPtr object, or just make sure to call Release when you are
    // done.

    // Creates a new object, use smart pointers to control ownership
    std::shared_ptr<PDFObject> ParseNewObject(ObjectIDType inObjectId);
    ObjectIDType GetObjectsCount() const;

    // Query a dictinary object, if indirect, go and fetch the indirect object and return it instead
    // [if you want the direct dictionary value, use PDFDictionary::QueryDirectObject [will AddRef automatically]
    std::shared_ptr<PDFObject> QueryDictionaryObject(const std::shared_ptr<PDFDictionary> &inDictionary,
                                                     const std::string &inName);

    // Query an array object, if indirect, go and fetch the indirect object and return it instead
    // [if you want the direct array value, use the PDFArray direct access to the vector [and use AddRef, cause it
    // won't]
    std::shared_ptr<PDFObject> QueryArrayObject(const std::shared_ptr<PDFArray> &inArray, unsigned long inIndex);

    unsigned long GetPagesCount() const;
    // don't be confused - pass number of pages here. returns the dictionary, and verifies that it's actually a page
    // (via type)
    std::shared_ptr<PDFDictionary> ParsePage(unsigned long inPageIndex);
    // get page object ID for an input index
    ObjectIDType GetPageObjectID(unsigned long inPageIndex);

    // Create a reader that will be able to read the stream. when filters are included
    // in the stream definition it will add them. delete the returned object when done.
    // Note that it DOES NOT setup the reading position of the file for reading the stream,
    // so if you want to read it, you have to also move the strem position, or use StartReadingFromStream instead
    charta::IByteReader *CreateInputStreamReader(const std::shared_ptr<PDFStreamInput> &inStream);

    /*
        Create a reader that will be able to read the stream, but without defiltering it.
        It will only decrypt it, if decryption is supported. This is ideal for copying
    */
    charta::IByteReader *CreateInputStreamReaderForPlainCopying(const std::shared_ptr<PDFStreamInput> &inStream);

    // prepare parser so that you can read from the input stream object.
    // create filters and move the stream to the beginning of the stream position.
    // delete the result when done
    charta::IByteReader *StartReadingFromStream(const std::shared_ptr<PDFStreamInput> &inStream);

    // creates a PDFObjectParser object that you can use for reading objects
    // from the input stream. very userful for reading content streams for
    // interpreting them
    PDFObjectParser *StartReadingObjectsFromStream(std::shared_ptr<PDFStreamInput> inStream);
    // same, but for an array of streams, in case of page contents that are arrays. need to count as one
    PDFObjectParser *StartReadingObjectsFromStreams(std::shared_ptr<PDFArray> inArrayOfStreams);

    /*
        Same as above, but reading only decrypts, but does not defiler. ideal for copying
    */
    charta::IByteReader *StartReadingFromStreamForPlainCopying(const std::shared_ptr<PDFStreamInput> &inStream);

    // use this to explictly free used objects. quite obviously this means that you'll have to parse the file again
    void ResetParser();

    // using PDFParser also for state information reading. this is a specialized version of the StartParsing for reading
    // state
    charta::EStatusCode StartStateFileParsing(charta::IByteReaderWithPosition *inSourceStream);

    // check if this file is encrypted. considering that the library can't really handle these files, this shoud be
    // handy.
    bool IsEncrypted();
    // encryption is supported if there's an extender that supports it in the parser
    bool IsEncryptionSupported();

    // set extender for parser, to enhance parsing capabilities
    void SetParserExtender(IPDFParserExtender *inParserExtender);

    // advanced, direct xref access
    ObjectIDType GetXrefSize() const;
    XrefEntryInput *GetXrefEntry(ObjectIDType inObjectID);
    long long GetXrefPosition() const;

    charta::IByteReaderWithPosition *GetParserStream();

  private:
    PDFObjectParser mObjectParser;
    DecryptionHelper mDecryptionHelper;
    charta::IByteReaderWithPosition *mStream;
    AdapterIByteReaderWithPositionToIReadPositionProvider mCurrentPositionProvider;

    // we'll use this items for bacwkards reading. might turns this into a proper stream object
    uint8_t mLinesBuffer[LINE_BUFFER_SIZE];
    uint8_t *mCurrentBufferIndex;
    uint8_t *mLastAvailableIndex;
    size_t mLastReadPositionFromEnd;
    bool mEncounteredFileStart;
    ObjectIDTypeToObjectStreamHeaderEntryMap mObjectStreamsCache;

    double mPDFLevel;
    long long mLastXrefPosition;
    std::shared_ptr<PDFDictionary> mTrailer;
    ObjectIDType mXrefSize;
    XrefEntryInput *mXrefTable;
    unsigned long mPagesCount;
    ObjectIDType *mPagesObjectIDs;
    IPDFParserExtender *mParserExtender;
    bool mAllowExtendingSegments;

    charta::EStatusCode ParseHeaderLine();
    charta::EStatusCode ParseEOFLine();
    charta::EStatusCode ParseLastXrefPosition();
    charta::EStatusCode ParseTrailerDictionary(std::shared_ptr<PDFDictionary> *outTrailer);
    charta::EStatusCode BuildXrefTableFromTable();
    charta::EStatusCode DetermineXrefSize();
    charta::EStatusCode InitializeXref();
    charta::EStatusCode ParseXrefFromXrefTable(XrefEntryInput *inXrefTable, ObjectIDType inXrefSize,
                                               long long inXrefPosition, bool inIsFirstXref,
                                               XrefEntryInput **outExtendedTable, ObjectIDType *outExtendedTableSize);
    XrefEntryInput *ExtendXrefTableToSize(XrefEntryInput *inXrefTable, ObjectIDType inOldSize, ObjectIDType inNewSize);
    charta::EStatusCode ReadNextXrefEntry(uint8_t inBuffer[20]);
    std::shared_ptr<PDFObject> ParseExistingInDirectObject(ObjectIDType inObjectID);
    charta::EStatusCode SetupDecryptionHelper(const std::string &inPassword);
    charta::EStatusCode ParsePagesObjectIDs();
    charta::EStatusCode ParsePagesIDs(std::shared_ptr<PDFDictionary> inPageNode, ObjectIDType inNodeObjectID);
    charta::EStatusCode ParsePagesIDs(const std::shared_ptr<PDFDictionary> &inPageNode, ObjectIDType inNodeObjectID,
                                      unsigned long &ioCurrentPageIndex);
    charta::EStatusCode ParsePreviousXrefs(const std::shared_ptr<PDFDictionary> &inTrailer);
    void MergeXrefWithMainXref(XrefEntryInput *inTableToMerge, ObjectIDType inMergedTableSize);
    charta::EStatusCode ParseFileDirectory();
    charta::EStatusCode BuildXrefTableAndTrailerFromXrefStream(long long inXrefStreamObjectID);
    // an overload for cases where the xref stream object is already parsed
    charta::EStatusCode ParseXrefFromXrefStream(XrefEntryInput *inXrefTable, ObjectIDType inXrefSize,
                                                const std::shared_ptr<PDFStreamInput> &inXrefStream,
                                                XrefEntryInput **outExtendedTable, ObjectIDType *outExtendedTableSize);
    // an overload for cases where the position should hold a stream object, and it should be parsed
    charta::EStatusCode ParseXrefFromXrefStream(XrefEntryInput *inXrefTable, ObjectIDType inXrefSize,
                                                long long inXrefPosition, XrefEntryInput **outExtendedTable,
                                                ObjectIDType *outExtendedTableSize);
    charta::EStatusCode ReadXrefStreamSegment(XrefEntryInput *inXrefTable, ObjectIDType inSegmentStartObject,
                                              ObjectIDType inSegmentCount, charta::IByteReader *inReadFrom,
                                              int *inEntryWidths, unsigned long inEntryWidthsSize);
    charta::EStatusCode ReadXrefSegmentValue(charta::IByteReader *inSource, int inEntrySize, long long &outValue);
    charta::EStatusCode ReadXrefSegmentValue(charta::IByteReader *inSource, int inEntrySize, ObjectIDType &outValue);
    charta::EStatusCode ParsePreviousFileDirectory(long long inXrefPosition, XrefEntryInput *inXrefTable,
                                                   ObjectIDType inXrefSize, std::shared_ptr<PDFDictionary> *outTrailer,
                                                   XrefEntryInput **outExtendedTable,
                                                   ObjectIDType *outExtendedTableSize);
    std::shared_ptr<PDFObject> ParseExistingInDirectStreamObject(ObjectIDType inObjectId);
    charta::EStatusCode ParseObjectStreamHeader(ObjectStreamHeaderEntry *inHeaderInfo, ObjectIDType inObjectsCount);
    void MovePositionInStream(long long inPosition);
    EStatusCodeAndIByteReader CreateFilterForStream(charta::IByteReader *inStream,
                                                    const std::shared_ptr<PDFName> &inFilterName,
                                                    const std::shared_ptr<PDFDictionary> &inDecodeParams,
                                                    const std::shared_ptr<PDFStreamInput> &inPDFStream);

    void NotifyIndirectObjectStart(long long inObjectID, long long inGenerationNumber);
    void NotifyIndirectObjectEnd(const std::shared_ptr<PDFObject> &inObject);

    charta::IByteReader *WrapWithDecryptionFilter(const std::shared_ptr<PDFStreamInput> &inStream,
                                                  charta::IByteReader *inToWrapStream);

    // Backward reading
    bool ReadNextBufferFromEnd();
    size_t GetCurrentPositionFromEnd();
    bool ReadBack(uint8_t &outValue);
    bool IsBeginOfFile();

    bool GoBackTillToken();
    bool GoBackTillNonToken();
    void GoBackTillLineStart();
    bool IsPDFWhiteSpace(uint8_t inCharacter);
};
