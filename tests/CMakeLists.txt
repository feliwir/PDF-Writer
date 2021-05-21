add_executable(libcharta_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/AppendingAndReadingTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AppendPagesTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AppendSpecialPagesTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BasicModificationTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BoxingBaseTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BufferedOutputStreamTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CopyingAndMergingEmptyPagesTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CustomLogTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DCTDecodeFilterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DFontTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EmptyFileTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EmptyPagesPDFTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EncryptedPDFTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FlateEncryptionTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FlateObjectDecodeTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FormXObjectTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeTypeInitializationTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/HighLevelContentContextTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/HighLevelImagesTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ImagesAndFormsForwardReferenceTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InputFlateDecodeTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InputImagesAsStreamsTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/JPGImageTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LinksTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LogTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MergePDFPagesTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MergeToPDFFormTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ModifyingEncryptedFileTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ModifyingExistingFileContentTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenTypeTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/OutputFileStreamTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PageModifierTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PageOrderModificationTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ParsingBadXrefTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ParsingFaultyTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFComment.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFCommentWriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFCommentWriter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFCopyingContextTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFDateTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFEmbedTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFObjectParserTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFParserTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFTextStringTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFWithPasswordTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PFBStreamTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PNGImageTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RecryptPDFTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RotatedPagesPDFTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ShutDownRestartTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleContentPageTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleTextUsageTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TestHelper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TextMeasurementsTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TIFFImageTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TIFFSpecialsTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TrueTypeTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TTCTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Type1Test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UnicodeTextUsageTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UppercaseSequenceTest.cpp

 )

if(BUILD_COVERAGE)
    setup_target_for_coverage_lcov(
        NAME libcharta_coverage
        EXECUTABLE ctest -j ${n_cores}
        EXCLUDE "contrib/*" "/usr/include/*"
        DEPENDENCIES libcharta_tests)
endif()

target_compile_definitions(libcharta_tests PRIVATE "-DPDFWRITE_SOURCE_PATH=\"${CMAKE_SOURCE_DIR}\" ")
target_compile_definitions(libcharta_tests PRIVATE "-DPDFWRITE_BINARY_PATH=\"${CMAKE_CURRENT_BINARY_DIR}\" ")

target_link_libraries(libcharta_tests PRIVATE gtest gtest_main libcharta -lstdc++fs)

include(GoogleTest)
gtest_discover_tests(libcharta_tests)