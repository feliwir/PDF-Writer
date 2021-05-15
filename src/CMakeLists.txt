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


set(PDF_WRITER_SOURCE
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
    ParsedPrimitiveHelper.cpp
    PDFDate.cpp
    PDFFormXObject.cpp
    PDFTiledPattern.cpp
    TiledPatternContentContext.cpp
    PDFImageXObject.cpp
    PDFModifiedPage.cpp
    PDFPage.cpp
    PDFPageInput.cpp
    PDFDictionaryIterator.cpp
    PDFArrayIterator.cpp
    PDFRectangle.cpp
    PDFStream.cpp
    PDFTextString.cpp
    PDFUsedFont.cpp
    PDFWriter.cpp
    PrimitiveObjectsWriter.cpp
    PSBool.cpp
    RefCountObject.cpp
    ResourcesDictionary.cpp
    StateReader.cpp
    StateWriter.cpp
    Timer.cpp
    TimersRegistry.cpp
    Trace.cpp
    TrailerInformation.cpp
    UppercaseSequance.cpp
    UsedFontsRepository.cpp
    WrittenFontCFF.cpp
    WrittenFontTrueType.cpp
    XObjectContentContext.cpp
)

set(PDF_WRITER_HEADERS
    #headers
    AbstractContentContext.h
    AbstractWrittenFont.h
    ANSIFontWriter.h
    BetweenIncluding.h
    BoxingBase.h
    CatalogInformation.h
    CIDFontWriter.h
    CMYKRGBColor.h
    ContainerIterator.h
    DescendentFontWriter.h
    DictionaryContext.h
    DictOperand.h
    DocumentContext.h
    DocumentContextExtenderAdapter.h
    EHummusImageType.h
    EPDFVersion.h
    EStatusCode.h
    ETokenSeparator.h
    FontDescriptorWriter.h
    FSType.h
    GlyphUnicodeMapping.h
    GraphicState.h
    GraphicStateStack.h
    IANSIFontWriterHelper.h
    IContentContextListener.h
    IDescendentFontWriter.h
    IDocumentContextExtender.h
    IFontDescriptorHelper.h
    IFormEndWritingTask.h
    ITiledPatternEndWritingTask.h
    IndirectObjectsReferenceRegistry.h
    InfoDictionary.h
    IObjectsContextExtender.h
    IPageEndWritingTask.h
    IResourceWritingTask.h
    IType1InterpreterImplementation.h
    IType2InterpreterImplementation.h
    IWrittenFont.h
    Log.h
    MapIterator.h
    MyStringBuf.h
    ObjectsBasicTypes.h
    ObjectsContext.h
    ObjectsContextExtenderAdapter.h
    PageContentContext.h
    PageTree.h
    ParsedPrimitiveHelper.h
    PDFDate.h
    PDFFormXObject.h
    PDFTiledPattern.h
    TiledPatternContentContext.h
    PDFImageXObject.h
    PDFModifiedPage.h
    PDFPage.h
    PDFPageInput.h
    PDFDictionaryIterator.h
    PDFArrayIterator.h
    PDFRectangle.h
    PDFStream.h
    PDFTextString.h
    PDFUsedFont.h
    PDFWriter.h
    PrimitiveObjectsWriter.h
    ProcsetResourcesConstants.h
    PSBool.h
    RefCountObject.h
    RefCountPtr.h
    ResourcesDictionary.h
    SafeBufferMacrosDefs.h
    Singleton.h
    SingleValueContainerIterator.h
    StateReader.h
    StateWriter.h
    Timer.h
    TimersRegistry.h
    Trace.h
    TrailerInformation.h
    UppercaseSequance.h
    UsedFontsRepository.h
    WrittenFontCFF.h
    WrittenFontRepresentation.h
    WrittenFontTrueType.h
    XObjectContentContext.h
)

add_library(PDFWriter ${PDF_WRITER_SOURCE} ${PDF_WRITER_HEADERS})

add_subdirectory(encoding)
add_subdirectory(encryption)
add_subdirectory(io)
add_subdirectory(images)
add_subdirectory(objects)
add_subdirectory(parsing)
add_subdirectory(text)

target_link_libraries(PDFWriter PRIVATE aes JPEG::JPEG ZLIB::ZLIB PNG::PNG TIFF::TIFF Freetype::Freetype)
target_include_directories(PDFWriter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(PDFWriter PUBLIC ${FREETYPE_INCLUDE_DIRS})