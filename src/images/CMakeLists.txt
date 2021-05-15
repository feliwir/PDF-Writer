target_sources(PDFWriter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/jpeg/JPEGImageHandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/jpeg/JPEGImageInformation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/jpeg/JPEGImageParser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/png/PNGImageHandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/tiff/TIFFImageHandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/tiff/TiffUsageParameters.h
)

target_sources(PDFWriter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/jpeg/JPEGImageHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/jpeg/JPEGImageInformation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/jpeg/JPEGImageParser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/png/PNGImageHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tiff/TIFFImageHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tiff/TiffUsageParameters.cpp
)