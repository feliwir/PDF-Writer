#include <cxxopts.hpp>
#include <iostream>
#include <libcharta/io/InputFile.h>
#include <libcharta/objects/PDFPageInput.h>
#include <libcharta/parsing/PDFParser.h>

int main(int argc, char **argv)
{
    cxxopts::Options options("pdfinfo", "Get meta-information about PDF files");

    // clang-format off
    options.add_options()
    ("version", "Version output")
    ("h,help", "Print usage");
    // clang-format on

    auto result = options.parse(argc, argv);
    if (result.unmatched().empty() || result.count("help") > 0u)
    {
        std::cout << options.help() << std::endl;
        return EXIT_SUCCESS;
    }

    if (result.count("version") > 0u)
    {
        std::cout << "TODO: print version info" << std::endl;
        return EXIT_SUCCESS;
    }

    auto input_files = result.unmatched();
    auto input = input_files.front();

    PDFParser parser;
    InputFile pdfFile;
    auto status = pdfFile.OpenFile(input);
    if (status != PDFHummus::eSuccess)
    {
        std::cerr << "Failed to open file: " << input << std::endl;
        return EXIT_FAILURE;
    }

    status = parser.StartPDFParsing(pdfFile.GetInputStream());
    if (status != PDFHummus::eSuccess)
    {
        std::cerr << "Failed to parse PDF file: " << input << std::endl;
        return EXIT_FAILURE;
    }

    PDFRectangle mediaBox;
    if (parser.GetPagesCount() > 0)
    {
        mediaBox = PDFPageInput(&parser, parser.ParsePage(0)).GetMediaBox();
    }

    std::cout << "Pages: " << parser.GetPagesCount() << std::endl;
    std::cout << "Encrypted: " << parser.IsEncrypted() << std::endl;
    std::cout << "Page size: " << mediaBox.UpperRightX << " x " << mediaBox.UpperRightY << " pts" << std::endl;
    std::cout << "PDF version: " << parser.GetPDFLevel() << std::endl;

    return EXIT_SUCCESS;
}