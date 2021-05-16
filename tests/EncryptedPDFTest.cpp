#include "PDFWriter.h"
#include "TestHelper.h"
#include "io/InputFile.h"
#include "parsing/PDFDocumentCopyingContext.h"
#include "parsing/PDFParser.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

TEST(Xcryption, EncryptedPDF)
{
    EStatusCode status = PDFHummus::eSuccess;
    InputFile pdfFile;
    PDFWriter pdfWriter;
    PDFDocumentCopyingContext *copyingContext = nullptr;

    PDFParser parser;

    status = pdfWriter.StartPDF(
        RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "PDFWithPasswordDecrypted.pdf"), ePDFVersion13,
        LogConfiguration(true, true, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "PDFWithPasswordDecrypted.txt")));
    ASSERT_EQ(status, PDFHummus::eSuccess);

    copyingContext = pdfWriter.CreatePDFCopyingContext(
        RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/PDFWithPassword.pdf"), PDFParsingOptions("user"));
    ASSERT_NE(copyingContext, nullptr);

    unsigned long pagesCount = copyingContext->GetSourceDocumentParser()->GetPagesCount();
    for (unsigned long i = 0; i < pagesCount && status == PDFHummus::eSuccess; ++i)
    {
        EStatusCodeAndObjectIDType result = copyingContext->AppendPDFPageFromPDF(i);
        ASSERT_EQ(result.first, PDFHummus::eSuccess);
    }

    delete copyingContext;
    copyingContext = nullptr;

    status = pdfWriter.EndPDF();
    ASSERT_EQ(status, PDFHummus::eSuccess);

    delete copyingContext;
}