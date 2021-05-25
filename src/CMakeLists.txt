option(PDFWRITER_SUPPORT_JPG "Wether or not JPEG/DCT support is enabled" TRUE)
option(PDFWRITER_SUPPORT_PNG "Wether or not PNG support is enabled" TRUE)
option(PDFWRITER_SUPPORT_TIFF "Wether or not TIFF support is enabled" TRUE)

find_package(ZLIB REQUIRED)
find_package(Freetype REQUIRED)

if(PDFWRITER_SUPPORT_JPG)
    find_package(JPEG REQUIRED)
else()
	add_definitions(-DPDFHUMMUS_NO_DCT=1)
endif()

if(PDFWRITER_SUPPORT_TIFF)
    find_package(TIFF REQUIRED)
else()
	add_definitions(-DPDFHUMMUS_NO_TIFF=1)
endif()

if(PDFWRITER_SUPPORT_PNG)
    find_package(PNG REQUIRED)
else()
	add_definitions(-DPDFHUMMUS_NO_PNG=1)
endif()

set(LIBCHARTA_SOURCE
    #sources
    AbstractContentContext.cpp
    AbstractWrittenFont.cpp
    ANSIFontWriter.cpp
    CatalogInformation.cpp
    CIDFontWriter.cpp
    CMYKRGBColor.cpp
    DescendentFontWriter.cpp
    DictionaryContext.cpp
    DocumentContext.cpp
    FontDescriptorWriter.cpp
    GraphicState.cpp
    GraphicStateStack.cpp
    IndirectObjectsReferenceRegistry.cpp
    InfoDictionary.cpp
    Log.cpp
    ObjectsContext.cpp
    PageContentContext.cpp
    PageTree.cpp
    PDFDate.cpp
    PDFFormXObject.cpp
    PDFTiledPattern.cpp
    TiledPatternContentContext.cpp
    PDFImageXObject.cpp
    PDFModifiedPage.cpp
    PDFPage.cpp
    PDFRectangle.cpp
    PDFStream.cpp
    PDFTextString.cpp
    PDFUsedFont.cpp
    PDFWriter.cpp
    PrimitiveObjectsWriter.cpp
    PSBool.cpp
    ResourcesDictionary.cpp
    StateReader.cpp
    StateWriter.cpp
    Trace.cpp
    TrailerInformation.cpp
    UppercaseSequence.cpp
    UsedFontsRepository.cpp
    WrittenFontCFF.cpp
    WrittenFontTrueType.cpp
    XObjectContentContext.cpp
)

set(LIBCHARTA_PUBLIC_INCLUDE_DIRECTORIES    ${CMAKE_SOURCE_DIR}/include/ ${CMAKE_SOURCE_DIR}/include/libcharta) # TODO: just point to include/
set(LIBCHARTA_PRIVATE_INCLUDE_DIRECTORIES   ${CMAKE_SOURCE_DIR}/src/   )

# Adding our target
add_library(libcharta ${LIBCHARTA_SOURCE} ${LIBCHARTA_PUBLIC_HEADERS})

add_subdirectory(encoding)
add_subdirectory(encryption)
add_subdirectory(io)
add_subdirectory(images)
add_subdirectory(objects)
add_subdirectory(parsing)
add_subdirectory(text)

target_include_directories(libcharta PUBLIC ${LIBCHARTA_PUBLIC_INCLUDE_DIRECTORIES})
target_include_directories(libcharta PRIVATE ${LIBCHARTA_PRIVATE_INCLUDE_DIRECTORIES}) 
target_include_directories(libcharta PUBLIC ${FREETYPE_INCLUDE_DIRS})

target_link_libraries(libcharta PRIVATE aes JPEG::JPEG ZLIB::ZLIB PNG::PNG TIFF::TIFF Freetype::Freetype)

# Installing
if(NOT SKIP_INSTALL_ALL )
    install(TARGETS libcharta   
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} 
        FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR})
    foreach ( file ${LIBCHARTA_PUBLIC_HEADERS} )
        file(RELATIVE_PATH rel_path ${CMAKE_SOURCE_DIR}/include ${file})
        get_filename_component( dir ${rel_path} DIRECTORY )
        install( FILES ${file} DESTINATION include/${dir} )
    endforeach()
endif()