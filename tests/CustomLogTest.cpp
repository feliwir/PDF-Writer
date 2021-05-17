#include "PDFPage.h"
#include "PDFWriter.h"
#include "Singleton.h"
#include "TestHelper.h"
#include "Trace.h"
#include "io/OutputFile.h"
#include "io/OutputFlateDecodeStream.h"
#include "io/OutputFlateEncodeStream.h"
#include "io/OutputStreamTraits.h"
#include "io/OutputStringBufferStream.h"
#include <stdint.h>
#include <stdio.h>

#include <gtest/gtest.h>
#include <string>

using namespace PDFHummus;

TEST(CustomStreams, CustomLog)
{
    // Place log in a compressed stream, for a non-file PDF
    EStatusCode status;
    OutputFlateEncodeStream flateEncodeStream;
    OutputFlateDecodeStream flateDecodeStream;

    PDFWriter pdfWriter;
    OutputFile compressedLogFile;
    OutputStringBufferStream pdfStream;

    // setup log file with compression
    status = compressedLogFile.OpenFile(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "CustomLogEncrypted.txt"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    flateEncodeStream.Assign(compressedLogFile.GetOutputStream());

    // generate PDF
    TRACE_LOG("Starting PDF File Writing");
    status = pdfWriter.StartPDFForStream(&pdfStream, ePDFVersion13, LogConfiguration(true, &flateEncodeStream));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    TRACE_LOG("Now will add an empty page");
    PDFPage page;

    page.SetMediaBox(PDFRectangle(0, 0, 400, 400));

    status = pdfWriter.WritePage(page);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    TRACE_LOG("Added page, now will close");

    status = pdfWriter.EndPDFForStream();
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // since log was started by starting PDF...the ending resets it. so let's now begin again
    Singleton<Trace>::GetInstance()->SetLogSettings(&flateEncodeStream, true);
    TRACE_LOG("Finished PDF!!!1");

    // dump PDF to a file, so we can review it
    OutputFile pdfFile;
    status = pdfFile.OpenFile(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "DumpPDFFile.pdf"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    auto pdfString = pdfStream.ToString();
    pdfFile.GetOutputStream()->Write((const uint8_t *)pdfString.c_str(), pdfString.size());
    pdfFile.CloseFile();

    TRACE_LOG("PDF stream dumped");

    // now finalize trace compressed file
    flateEncodeStream.Assign(nullptr);
    compressedLogFile.CloseFile();

    // Finish log
    Singleton<Trace>::Reset();

    // now open a new file and decompress the log into it.
    OutputFile decryptedLogFile;

    status = decryptedLogFile.OpenFile(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "CustomLogDecrypted.txt"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    // place an initial bom (cause the compressed content is unicode)
    unsigned short bom = (0xFE << 8) + 0xFF;
    decryptedLogFile.GetOutputStream()->Write((const uint8_t *)&bom, 2);

    flateDecodeStream.Assign(decryptedLogFile.GetOutputStream());
    OutputStreamTraits traits(&flateDecodeStream);

    InputFile compressedLogFileInput;
    status = compressedLogFileInput.OpenFile(RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "CustomLogEncrypted.txt"));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = traits.CopyToOutputStream(compressedLogFileInput.GetInputStream());
    ASSERT_EQ(status, PDFHummus::eSuccess);

    compressedLogFileInput.CloseFile();
    flateDecodeStream.Assign(nullptr);
    decryptedLogFile.CloseFile();

    ASSERT_EQ(status, PDFHummus::eSuccess);
}