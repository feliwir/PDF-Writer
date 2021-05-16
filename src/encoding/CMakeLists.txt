target_sources(pdfwriter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Ascii7Encoding.h
    ${CMAKE_CURRENT_SOURCE_DIR}/StandardEncoding.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFDocEncoding.h
    ${CMAKE_CURRENT_SOURCE_DIR}/UnicodeString.h
    ${CMAKE_CURRENT_SOURCE_DIR}/WinAnsiEncoding.h
)

target_sources(pdfwriter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Ascii7Encoding.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StandardEncoding.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFDocEncoding.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UnicodeString.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WinAnsiEncoding.cpp
)