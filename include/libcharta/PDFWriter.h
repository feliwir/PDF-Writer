/*
   Source File : PDFWriter.h


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
/*
    Main class for PDF library. entry point for writing PDFs.


*/

#include "DocumentContext.h"
#include "EPDFVersion.h"
#include "ObjectsContext.h"
#include "PDFRectangle.h"
#include "encryption/EncryptionOptions.h"
#include "images/tiff/TIFFUsageParameters.h"
#include "io/OutputFile.h"
#include "parsing/PDFEmbedParameterTypes.h"
#include "parsing/PDFParsingOptions.h"

#include <string>
#include <utility>

struct LogConfiguration
{
    bool ShouldLog;
    bool StartWithBOM;
    std::string LogFileLocation;
    charta::IByteWriter *LogStream;

    LogConfiguration(bool inShouldLog, bool inStartWithBOM, const std::string &inLogFileLocation)
    {
        ShouldLog = inShouldLog;
        StartWithBOM = inStartWithBOM;
        LogFileLocation = inLogFileLocation;
        LogStream = NULL;
    }
    LogConfiguration(bool inShouldLog, charta::IByteWriter *inLogStream)
    {
        ShouldLog = inShouldLog;
        LogStream = inLogStream;
        StartWithBOM = false;
    }

    static const LogConfiguration &DefaultLogConfiguration();
};

struct PDFCreationSettings
{
    bool CompressStreams;
    bool EmbedFonts;
    EncryptionOptions DocumentEncryptionOptions;

    PDFCreationSettings(bool inCompressStreams, bool inEmbedFonts,
                        EncryptionOptions inDocumentEncryptionOptions = EncryptionOptions::DefaultEncryptionOptions())
        : DocumentEncryptionOptions(inDocumentEncryptionOptions)
    {
        CompressStreams = inCompressStreams;
        EmbedFonts = inEmbedFonts;
    }
};

class PageContentContext;
class PDFFormXObject;
class PDFImageXObject;
class PDFUsedFont;
namespace charta
{
class IByteWriterWithPosition;
}

class PDFWriter
{
  public:
    PDFWriter();

    // output to file
    charta::EStatusCode StartPDF(
        const std::string &inOutputFilePath, EPDFVersion inPDFVersion,
        const LogConfiguration &inLogConfiguration = LogConfiguration::DefaultLogConfiguration(),
        const PDFCreationSettings &inPDFCreationSettings = PDFCreationSettings(true, true));

    charta::EStatusCode EndPDF();

    // output to stream
    charta::EStatusCode StartPDFForStream(
        charta::IByteWriterWithPosition *inOutputStream, EPDFVersion inPDFVersion,
        const LogConfiguration &inLogConfiguration = LogConfiguration::DefaultLogConfiguration(),
        const PDFCreationSettings &inPDFCreationSettings = PDFCreationSettings(true, true));
    charta::EStatusCode EndPDFForStream();

    // in case of internal or external error, call this function to cleanup, in order to allow reuse of the PDFWriter
    // class
    void Reset();

    // modify PDF (use EndPDF to finish)
    charta::EStatusCode ModifyPDF(
        const std::string &inModifiedFile, EPDFVersion inPDFVersion, const std::string &inOptionalAlternativeOutputFile,
        const LogConfiguration &inLogConfiguration = LogConfiguration::DefaultLogConfiguration(),
        const PDFCreationSettings &inPDFCreationSettings = PDFCreationSettings(true, true));
    charta::EStatusCode ModifyPDFForStream(
        charta::IByteReaderWithPosition *inModifiedSourceStream,
        charta::IByteWriterWithPosition *inModifiedDestinationStream, bool inAppendOnly, EPDFVersion inPDFVersion,
        const LogConfiguration &inLogConfiguration = LogConfiguration::DefaultLogConfiguration(),
        const PDFCreationSettings &inPDFCreationSettings = PDFCreationSettings(true, true));

    // Ending and Restarting writing session (optional input file is for modification scenarios)
    charta::EStatusCode Shutdown(const std::string &inStateFilePath);
    charta::EStatusCode ContinuePDF(
        const std::string &inOutputFilePath, const std::string &inStateFilePath,
        const std::string &inOptionalModifiedFile = "",
        const LogConfiguration &inLogConfiguration = LogConfiguration::DefaultLogConfiguration());
    // Continue PDF in output stream workflow (optional input stream is for modification scenarios)
    charta::EStatusCode ContinuePDFForStream(
        charta::IByteWriterWithPosition *inOutputStream, const std::string &inStateFilePath,
        charta::IByteReaderWithPosition *inModifiedSourceStream = NULL,
        const LogConfiguration &inLogConfiguration = LogConfiguration::DefaultLogConfiguration());

    // Page context, for drwaing page content
    PageContentContext *StartPageContentContext(PDFPage &inPage);
    charta::EStatusCode PausePageContentContext(PageContentContext *inPageContext);
    charta::EStatusCode EndPageContentContext(PageContentContext *inPageContext);

    // Page Writing [create a new Page by creating a new instance of PDFPage. instances may be reused.
    charta::EStatusCode WritePage(PDFPage &inPage);
    charta::EStatusCode WritePageAndRelease(PDFPage *inPage);

    // same as above page writing, but also return page ID. good for extensibility, when you want to refer to the
    // written page form some other place
    EStatusCodeAndObjectIDType WritePageAndReturnPageID(PDFPage &inPage);
    EStatusCodeAndObjectIDType WritePageReleaseAndReturnPageID(PDFPage *inPage);

    // Form XObject creating and writing
    PDFFormXObject *StartFormXObject(const PDFRectangle &inBoundingBox, const double *inMatrix = NULL);
    PDFFormXObject *StartFormXObject(const PDFRectangle &inBoundingBox, ObjectIDType inFormXObjectID,
                                     const double *inMatrix = NULL);
    charta::EStatusCode EndFormXObject(PDFFormXObject *inFormXObject);
    charta::EStatusCode EndFormXObjectAndRelease(PDFFormXObject *inFormXObject);

    // Image XObject creating [for TIFF nad JPG files].
    // note that as oppose to other methods, create the image xobject also writes it, so there's no
    // "WriteXXXXAndRelease" for image. So...release the object yourself [just delete it]

    // jpeg - two variants
    // will return image xobject sized at 1X1
    PDFImageXObject *CreateImageXObjectFromJPGFile(const std::string &inJPGFilePath);
    PDFImageXObject *CreateImageXObjectFromJPGStream(charta::IByteReaderWithPosition *inJPGStream);
    PDFImageXObject *CreateImageXObjectFromJPGFile(const std::string &inJPGFilePath, ObjectIDType inImageXObjectID);
    PDFImageXObject *CreateImageXObjectFromJPGStream(charta::IByteReaderWithPosition *inJPGStream,
                                                     ObjectIDType inImageXObjectID);

    // will return form XObject, which will include the xobject at it's size.
    // size is determined by the following order:
    // - JFIF resolution information is looked for. if found used to determine the size
    // - if not found. EXIF resolution information is looked for. if found used to determine the size
    // - if not found. Photoshop resolution information is looked for. if found used to determine the size
    // - otherwise aspect ratio is assumed, and so size is determined trivially from the samples width and height.
    PDFFormXObject *CreateFormXObjectFromJPGFile(const std::string &inJPGFilePath);
    PDFFormXObject *CreateFormXObjectFromJPGStream(charta::IByteReaderWithPosition *inJPGStream);
    PDFFormXObject *CreateFormXObjectFromJPGFile(const std::string &inJPGFilePath, ObjectIDType inFormXObjectID);
    PDFFormXObject *CreateFormXObjectFromJPGStream(charta::IByteReaderWithPosition *inJPGStream,
                                                   ObjectIDType inFormXObjectID);

    // tiff
#ifndef PDFHUMMUS_NO_TIFF
    PDFFormXObject *CreateFormXObjectFromTIFFFile(const std::string &inTIFFFilePath,
                                                  const charta::TIFFUsageParameters &inTIFFUsageParameters =
                                                      charta::TIFFUsageParameters::DefaultTIFFUsageParameters());
    PDFFormXObject *CreateFormXObjectFromTIFFStream(charta::IByteReaderWithPosition *inTIFFStream,
                                                    const charta::TIFFUsageParameters &inTIFFUsageParameters =
                                                        charta::TIFFUsageParameters::DefaultTIFFUsageParameters());
    PDFFormXObject *CreateFormXObjectFromTIFFFile(const std::string &inTIFFFilePath, ObjectIDType inFormXObjectID,
                                                  const charta::TIFFUsageParameters &inTIFFUsageParameters =
                                                      charta::TIFFUsageParameters::DefaultTIFFUsageParameters());
    PDFFormXObject *CreateFormXObjectFromTIFFStream(charta::IByteReaderWithPosition *inTIFFStream,
                                                    ObjectIDType inFormXObjectID,
                                                    const charta::TIFFUsageParameters &inTIFFUsageParameters =
                                                        charta::TIFFUsageParameters::DefaultTIFFUsageParameters());
#endif

    // png
#ifndef PDFHUMMUS_NO_PNG
    PDFFormXObject *CreateFormXObjectFromPNGFile(const std::string &inPNGFilePath);
    PDFFormXObject *CreateFormXObjectFromPNGFile(const std::string &inPNGFilePath, ObjectIDType inFormXObjectID);
    PDFFormXObject *CreateFormXObjectFromPNGStream(charta::IByteReaderWithPosition *inPNGStream);
    PDFFormXObject *CreateFormXObjectFromPNGStream(charta::IByteReaderWithPosition *inPNGStream,
                                                   ObjectIDType inFormXObjectID);
#endif

    // PDF

    // CreateFormXObjectsFromPDF is for using input PDF pages as objects in one page or more. you can used the returned
    // IDs to place the created form xobjects
    EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(
        const std::string &inPDFFilePath, const PDFPageRange &inPageRange, EPDFPageBox inPageBoxToUseAsFormBox,
        const double *inTransformationMatrix = NULL,
        const ObjectIDTypeList &inCopyAdditionalObjects = ObjectIDTypeList(),
        const PDFParsingOptions &inParsingOptions = PDFParsingOptions::DefaultPDFParsingOptions());

    EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(
        charta::IByteReaderWithPosition *inPDFStream, const PDFPageRange &inPageRange,
        EPDFPageBox inPageBoxToUseAsFormBox, const double *inTransformationMatrix = NULL,
        const ObjectIDTypeList &inCopyAdditionalObjects = ObjectIDTypeList(),
        const PDFParsingOptions &inParsingOptions = PDFParsingOptions::DefaultPDFParsingOptions());

    // CreateFormXObjectsFromPDF is an override to allow you to determine a custom crop for the page embed
    EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(
        const std::string &inPDFFilePath, const PDFPageRange &inPageRange, const PDFRectangle &inCropBox,
        const double *inTransformationMatrix = NULL,
        const ObjectIDTypeList &inCopyAdditionalObjects = ObjectIDTypeList(),
        const PDFParsingOptions &inParsingOptions = PDFParsingOptions::DefaultPDFParsingOptions());

    EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(
        charta::IByteReaderWithPosition *inPDFStream, const PDFPageRange &inPageRange, const PDFRectangle &inCropBox,
        const double *inTransformationMatrix = NULL,
        const ObjectIDTypeList &inCopyAdditionalObjects = ObjectIDTypeList(),
        const PDFParsingOptions &inParsingOptions = PDFParsingOptions::DefaultPDFParsingOptions());

    // AppendPDFPagesFromPDF is for simple appending of the input PDF pages
    EStatusCodeAndObjectIDTypeList AppendPDFPagesFromPDF(
        const std::string &inPDFFilePath, const PDFPageRange &inPageRange,
        const ObjectIDTypeList &inCopyAdditionalObjects = ObjectIDTypeList(),
        const PDFParsingOptions &inParsingOptions = PDFParsingOptions::DefaultPDFParsingOptions());

    EStatusCodeAndObjectIDTypeList AppendPDFPagesFromPDF(
        charta::IByteReaderWithPosition *inPDFStream, const PDFPageRange &inPageRange,
        const ObjectIDTypeList &inCopyAdditionalObjects = ObjectIDTypeList(),
        const PDFParsingOptions &inParsingOptions = PDFParsingOptions::DefaultPDFParsingOptions());

    // MergePDFPagesToPage, merge PDF pages content to an input page. good for single-placement of a page content,
    // cheaper than creating and XObject and later placing, when the intention is to use this graphic just once.
    charta::EStatusCode MergePDFPagesToPage(
        PDFPage &inPage, const std::string &inPDFFilePath, const PDFPageRange &inPageRange,
        const ObjectIDTypeList &inCopyAdditionalObjects = ObjectIDTypeList(),
        const PDFParsingOptions &inParsingOptions = PDFParsingOptions::DefaultPDFParsingOptions());

    charta::EStatusCode MergePDFPagesToPage(
        PDFPage &inPage, charta::IByteReaderWithPosition *inPDFStream, const PDFPageRange &inPageRange,
        const ObjectIDTypeList &inCopyAdditionalObjects = ObjectIDTypeList(),
        const PDFParsingOptions &inParsingOptions = PDFParsingOptions::DefaultPDFParsingOptions());

    // Copying context, allowing for a continous flow of copying from multiple sources PDFs (create one per source) to
    // target PDF
    std::shared_ptr<PDFDocumentCopyingContext> CreatePDFCopyingContext(
        const std::string &inPDFFilePath,
        const PDFParsingOptions &inOptions = PDFParsingOptions::DefaultPDFParsingOptions());
    std::shared_ptr<PDFDocumentCopyingContext> CreatePDFCopyingContext(
        charta::IByteReaderWithPosition *inPDFStream,
        const PDFParsingOptions &inOptions = PDFParsingOptions::DefaultPDFParsingOptions());

    // for modified file path, create a copying context for the modified file
    std::shared_ptr<PDFDocumentCopyingContext> CreatePDFCopyingContextForModifiedFile();

    // some public image info services, for users of hummus
    std::pair<double, double> GetImageDimensions(
        const std::string &inImageFile, unsigned long inImageIndex = 0,
        const PDFParsingOptions &inParsingOptions = PDFParsingOptions::DefaultPDFParsingOptions());
    std::pair<double, double> GetImageDimensions(
        charta::IByteReaderWithPosition *inImageStream, unsigned long inImageIndex = 0,
        const PDFParsingOptions &inParsingOptions = PDFParsingOptions::DefaultPDFParsingOptions());
    charta::EHummusImageType GetImageType(const std::string &inImageFile, unsigned long inImageIndex);
    unsigned long GetImagePagesCount(const std::string &inImageFile, const PDFParsingOptions &inOptions =
                                                                         PDFParsingOptions::DefaultPDFParsingOptions());

    // fonts [text], font index is provided for multi-font file packages (such as dfont and ttc), 0 the default is
    // what should be passed for single-font files
    PDFUsedFont *GetFontForFile(const std::string &inFontFilePath, long inFontIndex = 0);
    // second overload is for type 1, when an additional metrics file is available
    PDFUsedFont *GetFontForFile(const std::string &inFontFilePath, const std::string &inAdditionalMeticsFilePath,
                                long inFontIndex = 0);

    // URL links
    // URL should be encoded to be a valid URL, ain't gonna be checking that!
    charta::EStatusCode AttachURLLinktoCurrentPage(const std::string &inURL, const PDFRectangle &inLinkClickArea);

    // Extensibility, reaching to lower levels
    inline charta::DocumentContext &GetDocumentContext()
    {
        return mDocumentContext;
    }

    inline ObjectsContext &GetObjectsContext()
    {
        return mObjectsContext;
    }

    inline charta::OutputFile &GetOutputFile()
    {
        return mOutputFile;
    }

    // Extensibiility, for modified files workflow
    inline PDFParser &GetModifiedFileParser()
    {
        return mModifiedFileParser;
    }

    inline charta::InputFile &GetModifiedInputFile()
    {
        return mModifiedFile;
    }

    // Recryption statics. create new version of an existing document encrypted with new password or decrypted
    static charta::EStatusCode RecryptPDF(const std::string &inOriginalPDFPath,
                                          const std::string &inOriginalPDFPassword, const std::string &inNewPDFPath,
                                          const LogConfiguration &inLogConfiguration,
                                          const PDFCreationSettings &inPDFCreationSettings,
                                          EPDFVersion inOveridePDFVersion = ePDFVersionUndefined);

    static charta::EStatusCode RecryptPDF(charta::IByteReaderWithPosition *inOriginalPDFStream,
                                          const std::string &inOriginalPDFPassword,
                                          charta::IByteWriterWithPosition *inNewPDFStream,
                                          const LogConfiguration &inLogConfiguration,
                                          const PDFCreationSettings &inPDFCreationSettings,
                                          EPDFVersion inOveridePDFVersion = ePDFVersionUndefined);

  private:
    ObjectsContext mObjectsContext;
    charta::DocumentContext mDocumentContext;

    // for output file workflow, this will be the valid output [stream workflow does not have a file]
    charta::OutputFile mOutputFile;

    // for modified workflow, the next two will hold the input file data
    charta::InputFile mModifiedFile;
    PDFParser mModifiedFileParser;
    EPDFVersion mModifiedFileVersion;
    bool mIsModified;

    void SetupLog(const LogConfiguration &inLogConfiguration);
    void SetupCreationSettings(const PDFCreationSettings &inPDFCreationSettings);
    void ReleaseLog();
    charta::EStatusCode SetupState(const std::string &inStateFilePath);
    void Cleanup();
    charta::EStatusCode SetupStateFromModifiedFile(const std::string &inModifiedFile, EPDFVersion inPDFVersion,
                                                   const PDFCreationSettings &inPDFCreationSettings);
    charta::EStatusCode SetupStateFromModifiedStream(charta::IByteReaderWithPosition *inModifiedSourceStream,
                                                     EPDFVersion inPDFVersion,
                                                     const PDFCreationSettings &inPDFCreationSettings);
};
