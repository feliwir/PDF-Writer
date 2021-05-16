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
 )

target_compile_definitions(pdfwriter_tests PRIVATE "-DPDFWRITE_SOURCE_PATH=\"${CMAKE_SOURCE_DIR}\" ")
target_compile_definitions(pdfwriter_tests PRIVATE "-DPDFWRITE_BINARY_PATH=\"${CMAKE_CURRENT_BINARY_DIR}\" ")

target_link_libraries(pdfwriter_tests PRIVATE gtest gtest_main pdfwriter -lstdc++fs)

include(GoogleTest)
gtest_discover_tests(pdfwriter_tests)