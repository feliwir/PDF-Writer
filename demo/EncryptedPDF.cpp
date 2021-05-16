#include "EncryptedPDF.h"
#include "PDFWriter.h"
#include "TestsRunner.h"
#include "io/InputFile.h"
#include "parsing/PDFDocumentCopyingContext.h"
#include "parsing/PDFParser.h"
#include <iostream>

using namespace std;
using namespace PDFHummus;

EncryptedPDFTester::EncryptedPDFTester()
{
}

EncryptedPDFTester::~EncryptedPDFTester()
{
}

EStatusCode EncryptedPDFTester::Run(const TestConfiguration &inTestConfiguration)
{
    EStatusCode status = PDFHummus::eSuccess;
    InputFile pdfFile;
    PDFWriter pdfWriter;
    PDFDocumentCopyingContext *copyingContext = nullptr;

    PDFParser parser;

    do
    {
        status = pdfWriter.StartPDF(
            RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "PDFWithPasswordDecrypted.pdf"), ePDFVersion13,
            LogConfiguration(
                true, true,
                RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "PDFWithPasswordDecrypted.txt")));
        if (status != PDFHummus::eSuccess)
        {
            cout << "failed to start PDF\n";
            break;
        }

        copyingContext = pdfWriter.CreatePDFCopyingContext(
            RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "data/PDFWithPassword.pdf"),
            PDFParsingOptions("user"));
        if (copyingContext == nullptr)
        {
            cout << "failed to initialize copying context from PDFWithPassword.pdf\n";
            status = PDFHummus::eFailure;
            break;
        }

        unsigned long pagesCount = copyingContext->GetSourceDocumentParser()->GetPagesCount();
        for (unsigned long i = 0; i < pagesCount && status == PDFHummus::eSuccess; ++i)
        {
            EStatusCodeAndObjectIDType result = copyingContext->AppendPDFPageFromPDF(i);
            if (result.first != PDFHummus::eSuccess)
            {
                cout << "failed to append page " << (i + 1) << " from PDFWithPassword.pdf\n";
                status = result.first;
                break;
            }
        }

        delete copyingContext;
        copyingContext = nullptr;

        status = pdfWriter.EndPDF();
        if (status != PDFHummus::eSuccess)
        {
            cout << "failed in end PDF\n";
            break;
        }

    } while (false);

    delete copyingContext;
    return status;
}

ADD_CATEGORIZED_TEST(EncryptedPDFTester, "Xcryption")