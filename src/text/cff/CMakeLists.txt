target_sources(PDFWriter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/CFFANSIFontWriter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CFFDescendentFontWriter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CFFEmbeddedFontWriter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CFFFileInput.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CFFPrimitiveReader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CFFPrimitiveWriter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CharStringDefinitions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CharStringType2Flattener.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CharStringType2Interpreter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CharStringType2Tracer.h
)

target_sources(PDFWriter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/CFFANSIFontWriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CFFDescendentFontWriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CFFEmbeddedFontWriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CFFFileInput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CFFPrimitiveReader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CFFPrimitiveWriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CharStringType2Flattener.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CharStringType2Interpreter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CharStringType2Tracer.cpp
)