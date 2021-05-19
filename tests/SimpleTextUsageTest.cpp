/*
   Source File : SimpleTextUsage.cpp


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
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PDFWriter.h"
#include "PageContentContext.h"
#include "TestHelper.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace PDFHummus;

EStatusCode RunCFFTest(bool inEmbedFonts)
{
    PDFWriter pdfWriter;
    EStatusCode status;

    status = pdfWriter.StartPDF(
        RelativeURLToLocalPath(PDFWRITE_BINARY_PATH,
                               inEmbedFonts ? "SimpleTextUsageCFF.pdf" : "SimpleTextUsageCFFNoEmbed.pdf"),
        ePDFVersion13,
        LogConfiguration(true, true, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "SimpleTextUsage.log")),
        PDFCreationSettings(true, inEmbedFonts));
    if (status != PDFHummus::eSuccess)
        return status;

    PDFPage page;
    page.SetMediaBox(PDFHummus::PagePresets::A4_Portrait);

    PageContentContext *contentContext = pdfWriter.StartPageContentContext(page);
    if (contentContext == nullptr)
        return PDFHummus::eFailure;

    PDFUsedFont *font =
        pdfWriter.GetFontForFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/BrushScriptStd.otf"));
    if (font == nullptr)
        return PDFHummus::eFailure;

    // Draw some text
    contentContext->BT();
    contentContext->k(0, 0, 0, 1);

    contentContext->Tf(font, 1);

    contentContext->Tm(30, 0, 0, 30, 78.4252, 662.8997);

    EStatusCode encodingStatus = contentContext->Tj("abcd");
    if (encodingStatus != PDFHummus::eSuccess)
        return encodingStatus;

    // continue even if failed...want to see how it looks like
    contentContext->ET();

    // now write some more text with kozuka font
    contentContext->BT();
    contentContext->k(0, 0, 0, 1);

    PDFUsedFont *fontK =
        pdfWriter.GetFontForFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/KozGoPro-Regular.otf"));
    if (fontK == nullptr)
        return PDFHummus::eFailure;

    contentContext->Tf(fontK, 1);

    contentContext->Tm(30, 0, 0, 30, 78.4252, 400.8997);

    encodingStatus = contentContext->Tj("abcd");
    if (encodingStatus != PDFHummus::eSuccess)
        return encodingStatus;

    // continue even if failed...want to see how it looks like
    contentContext->ET();

    status = pdfWriter.EndPageContentContext(contentContext);
    if (status != PDFHummus::eSuccess)
        return status;

    status = pdfWriter.WritePage(page);
    if (status != PDFHummus::eSuccess)
        return status;

    return pdfWriter.EndPDF();
}

EStatusCode RunTrueTypeTest(bool inEmbedFonts)
{
    PDFWriter pdfWriter;
    EStatusCode status;

    status = pdfWriter.StartPDF(
        RelativeURLToLocalPath(PDFWRITE_BINARY_PATH,
                               inEmbedFonts ? "SimpleTextUsageTrueType.pdf" : "SimpleTextUsageTrueTypeNoEmbed.pdf"),
        ePDFVersion13,
        LogConfiguration(true, true, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "SimpleTextUsage.log")),
        PDFCreationSettings(true, inEmbedFonts));
    if (status != PDFHummus::eSuccess)
        return status;

    PDFPage page;
    page.SetMediaBox(PDFHummus::PagePresets::A4_Portrait);

    PageContentContext *contentContext = pdfWriter.StartPageContentContext(page);
    if (contentContext == nullptr)
        return PDFHummus::eFailure;

    PDFUsedFont *font = pdfWriter.GetFontForFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/arial.ttf"));
    if (font == nullptr)
        return PDFHummus::eFailure;

    // Draw some text
    contentContext->BT();
    contentContext->k(0, 0, 0, 1);

    contentContext->Tf(font, 1);

    contentContext->Tm(30, 0, 0, 30, 78.4252, 662.8997);

    EStatusCode encodingStatus = contentContext->Tj("hello world");
    if (encodingStatus != PDFHummus::eSuccess)
        return encodingStatus;

    // continue even if failed...want to see how it looks like
    contentContext->ET();

    status = pdfWriter.EndPageContentContext(contentContext);
    if (status != PDFHummus::eSuccess)
        return status;

    status = pdfWriter.WritePage(page);
    if (status != PDFHummus::eSuccess)
        return status;

    return pdfWriter.EndPDF();
}

EStatusCode RunType1Test(bool inEmbedFonts)
{
    PDFWriter pdfWriter;
    EStatusCode status;

    status = pdfWriter.StartPDF(
        RelativeURLToLocalPath(PDFWRITE_BINARY_PATH,
                               inEmbedFonts ? "SimpleTextUsageType1.pdf" : "SimpleTextUsageType1NoEmbed.pdf"),
        ePDFVersion13,
        LogConfiguration(true, true, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "SimpleTextUsage.log")),
        PDFCreationSettings(true, inEmbedFonts));
    if (status != PDFHummus::eSuccess)
        return status;

    PDFPage page;
    page.SetMediaBox(PDFHummus::PagePresets::A4_Portrait);

    PageContentContext *contentContext = pdfWriter.StartPageContentContext(page);
    if (contentContext == nullptr)
        return PDFHummus::eFailure;

    PDFUsedFont *font =
        pdfWriter.GetFontForFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/HLB_____.PFB"),
                                 RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/HLB_____.PFM"));
    if (font == nullptr)
        return PDFHummus::eFailure;

    // Draw some text
    contentContext->BT();
    contentContext->k(0, 0, 0, 1);

    contentContext->Tf(font, 1);

    contentContext->Tm(30, 0, 0, 30, 78.4252, 662.8997);

    EStatusCode encodingStatus = contentContext->Tj("abcd \xC3\xA1"); // ending char is LATIN SMALL LETTER A WITH ACUTE
    if (encodingStatus != PDFHummus::eSuccess)
        return encodingStatus;

    // continue even if failed...want to see how it looks like
    contentContext->ET();

    status = pdfWriter.EndPageContentContext(contentContext);
    if (status != PDFHummus::eSuccess)
        return status;

    status = pdfWriter.WritePage(page);
    if (status != PDFHummus::eSuccess)
        return status;

    return pdfWriter.EndPDF();
}

EStatusCode RunNoTextTest(bool inEmbedFonts)
{
    // this one checks an edge case where a font object is created but no text written. should not fail.
    PDFWriter pdfWriter;
    EStatusCode status;

    status = pdfWriter.StartPDF(
        RelativeURLToLocalPath(PDFWRITE_BINARY_PATH,
                               inEmbedFonts ? "SimpleNoTextUsage.pdf" : "SimpleNoTextUsageNoEmbed.pdf"),
        ePDFVersion13,
        LogConfiguration(true, true, RelativeURLToLocalPath(PDFWRITE_BINARY_PATH, "SimpleTextUsage.log")),
        PDFCreationSettings(true, inEmbedFonts));
    if (status != PDFHummus::eSuccess)
        return status;

    PDFPage page;
    page.SetMediaBox(PDFHummus::PagePresets::A4_Portrait);

    PageContentContext *contentContext = pdfWriter.StartPageContentContext(page);
    if (contentContext == nullptr)
        status = PDFHummus::eFailure;

    PDFUsedFont *font =
        pdfWriter.GetFontForFile(RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/HLB_____.PFB"),
                                 RelativeURLToLocalPath(PDFWRITE_SOURCE_PATH, "data/fonts/HLB_____.PFM"));
    if (font == nullptr)
        return PDFHummus::eFailure;

    // Draw some text
    contentContext->BT();
    contentContext->k(0, 0, 0, 1);

    contentContext->Tf(font, 1);

    contentContext->Tm(30, 0, 0, 30, 78.4252, 662.8997);

    // no text is written!!!

    // continue even if failed...want to see how it looks like
    contentContext->ET();

    status = pdfWriter.EndPageContentContext(contentContext);
    if (status != PDFHummus::eSuccess)
        return status;

    status = pdfWriter.WritePage(page);
    if (status != PDFHummus::eSuccess)
        return status;

    return pdfWriter.EndPDF();
}

TEST(PDF, SimpleTextUsage)
{
    EStatusCode status;

    // running two versions of the tests, one with font embedding, one without

    status = RunCFFTest(true);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = RunCFFTest(false);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = RunTrueTypeTest(true);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = RunTrueTypeTest(false);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = RunType1Test(true);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = RunType1Test(false);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = RunNoTextTest(true);
    ASSERT_EQ(status, PDFHummus::eSuccess);

    status = RunNoTextTest(true);
    ASSERT_EQ(status, PDFHummus::eSuccess);
}
