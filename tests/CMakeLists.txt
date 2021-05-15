add_executable(pdfwrite_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/AppendPagesTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AppendSpecialPagesTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BoxingBaseTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BufferedOutputStreamTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CopyingAndMergingEmptyPagesTest.cpp
 )

target_compile_definitions(pdfwrite_tests PRIVATE "-DPDFWRITE_SOURCE_PATH=\"${CMAKE_SOURCE_DIR}\" ")
target_compile_definitions(pdfwrite_tests PRIVATE "-DPDFWRITE_BINARY_PATH=\"${CMAKE_CURRENT_BINARY_DIR}\" ")

target_link_libraries(pdfwrite_tests PRIVATE gtest gtest_main PDFWriter -lstdc++fs)

include(GoogleTest)
gtest_discover_tests(pdfwrite_tests)