target_sources(PDFWriter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/EFontStretch.h
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeTypeFaceWrapper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeTypeOpenTypeWrapper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeTypeType1Wrapper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeTypeWrapper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/IFreeTypeFaceExtender.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PFMFileReader.h
)

target_sources(PDFWriter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeTypeFaceWrapper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeTypeOpenTypeWrapper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeTypeType1Wrapper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeTypeWrapper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PFMFileReader.cpp
)