target_sources(PDFWriter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/IPDFParserExtender.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFDocumentCopyingContext.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFDocumentHandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFEmbedParameterTypes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFObjectParser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFPageMergingHelper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFParser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFParserTokenizer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFParsingOptions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleStringTokenizer.h
)

target_sources(PDFWriter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFDocumentCopyingContext.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFDocumentHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFObjectParser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFPageMergingHelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFParser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFParserTokenizer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PDFParsingOptions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleStringTokenizer.cpp
)