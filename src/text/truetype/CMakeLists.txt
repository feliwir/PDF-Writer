target_sources(PDFWriter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/TrueTypeANSIFontWriter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TrueTypeDescendentFontWriter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TrueTypeEmbeddedFontWriter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TrueTypePrimitiveWriter.h
)

target_sources(PDFWriter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/TrueTypeANSIFontWriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TrueTypeDescendentFontWriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TrueTypeEmbeddedFontWriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TrueTypePrimitiveWriter.cpp
)