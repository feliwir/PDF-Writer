target_sources(PDFWriter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/CharStringType1Interpreter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CharStringType1Tracer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Type1Input.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Type1ToCFFEmbeddedFontWriter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Type1ToType2Converter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Type2CharStringWriter.h
)

target_sources(PDFWriter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/CharStringType1Interpreter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CharStringType1Tracer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Type1Input.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Type1ToCFFEmbeddedFontWriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Type1ToType2Converter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Type2CharStringWriter.cpp
)