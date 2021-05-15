target_sources(PDFWriter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenTypeFileInput.h
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenTypePrimitiveReader.h
)

target_sources(PDFWriter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenTypeFileInput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenTypePrimitiveReader.cpp
)