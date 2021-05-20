target_sources(libcharta PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/jpeg/JPEGImageHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/jpeg/JPEGImageInformation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/jpeg/JPEGImageParser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/png/PNGImageHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tiff/TIFFImageHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tiff/TiffUsageParameters.cpp
)