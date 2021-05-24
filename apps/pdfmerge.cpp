#include <cxxopts.hpp>
#include <iostream>
#include <libcharta/PDFWriter.h>
#include <libcharta/parsing/PDFDocumentCopyingContext.h>

int main(int argc, char **argv)
{
    cxxopts::Options options("pdfmerge", "Merge multiple PDF files into a single one");

    // clang-format off
    options.add_options()
    ("d,debug", "Enable debugging", cxxopts::value<bool>()->default_value("false")) // a bool parameter
    ("o,output", "Output for the generated PDF", cxxopts::value<std::string>())
    ("v,version", "Version output")
    ("h,help", "Print usage");
    // clang-format on

    auto result = options.parse(argc, argv);
    if (result.arguments().empty() || result.count("help"))
    {
        std::cout << options.help() << std::endl;
        return EXIT_SUCCESS;
    }

    if (result.count("version"))
    {
        std::cout << "TODO: print version info" << std::endl;
        return EXIT_SUCCESS;
    }

    if (result.count("output") == 0)
    {
        std::cout << "Must specify an output file!" << std::endl;
        return EXIT_FAILURE;
    }

    auto output = result["output"].as<std::string>();

    PDFWriter pdfWriter;
    auto status = pdfWriter.StartPDF(output, ePDFVersion13, LogConfiguration(true, true, "AppendPagesTestLog.txt"));
    if (status != PDFHummus::eSuccess)
    {
        std::cerr << "Failed to start PDF: " << output << std::endl;
        return EXIT_FAILURE;
    }

    auto input_files = result.unmatched();
    std::cout << "Size: " << input_files.size() << std::endl;

    for (const auto &input : input_files)
    {
        std::cout << "Appending: " << input << std::endl;

        auto result = pdfWriter.AppendPDFPagesFromPDF(input, PDFPageRange());
        if (result.first != PDFHummus::eSuccess)
        {
            std::cerr << "Failed to append PDF: " << input << std::endl;
            return EXIT_FAILURE;
        }
    }

    status = pdfWriter.EndPDF();
    if (status != PDFHummus::eSuccess)
    {
        std::cerr << "Failed to end PDF: " << output << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}