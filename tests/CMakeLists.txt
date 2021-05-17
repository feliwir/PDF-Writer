add_executable(pdfwriter_tests
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
 )

if(BUILD_COVERAGE)
    setup_target_for_coverage_lcov(
        NAME pdfwriter_coverage
        EXECUTABLE ctest -j ${n_cores}
        EXCLUDE "demo/*" "contrib/*" "/usr/include/*"
        DEPENDENCIES pdfwriter_tests)
endif()

target_compile_definitions(pdfwriter_tests PRIVATE "-DPDFWRITE_SOURCE_PATH=\"${CMAKE_SOURCE_DIR}\" ")
target_compile_definitions(pdfwriter_tests PRIVATE "-DPDFWRITE_BINARY_PATH=\"${CMAKE_CURRENT_BINARY_DIR}\" ")

target_link_libraries(pdfwriter_tests PRIVATE gtest gtest_main pdfwriter -lstdc++fs)

include(GoogleTest)
gtest_discover_tests(pdfwriter_tests)