add_executable(PDFWriterTestPlayground 

#sources
RotatedPagesPDF.cpp
FileURL.cpp
PDFWithPassword.cpp
PageOrderModification.cpp
ParsingFaulty.cpp
PDFCopyingContextTest.cpp
PDFDateTest.cpp
PDFEmbedTest.cpp
PDFObjectCastTest.cpp
PDFObjectParserTest.cpp
PDFParserTest.cpp
PDFTextStringTest.cpp
PFBStreamTest.cpp
PNGImageTest.cpp
PosixPath.cpp
RecryptPDF.cpp
RefCountTest.cpp
ShutDownRestartTest.cpp
SimpleContentPageTest.cpp
SimpleTextUsage.cpp
TestMeasurementsTest.cpp
TestsRunner.cpp
TextUsageBugs.cpp
TIFFImageTest.cpp
TiffSpecialsTest.cpp
TimerTest.cpp
TrueTypeTest.cpp
TTCTest.cpp
Type1Test.cpp
UppercaseSequanceTest.cpp
WindowsPath.cpp
PDFWriterTestPlayground.cpp
UnicodeTextUsage.cpp

#headers
RotatedPagesPDF.h
FileURL.h
ITestUnit.h
PDFWithPassword.h
PageOrderModification.h
ParsingFaulty.h
PDFCopyingContextTest.h
PDFDateTest.h
PDFEmbedTest.h
PDFObjectCastTest.h
PDFObjectParserTest.h
PDFParserTest.h
PDFTextStringTest.h
PFBStreamTest.h
PNGImageTest.h
PosixPath.h
RecryptPDF.h
RefCountTest.h
ShutDownRestartTest.h
SimpleContentPageTest.h
SimpleTextUsage.h
TestMeasurementsTest.h
TestsRunner.h
TextUsageBugs.h
TIFFImageTest.h
TiffSpecialsTest.h
TimerTest.h
TrueTypeTest.h
TTCTest.h
Type1Test.h
UppercaseSequanceTest.h
UnicodeTextUsage.h
)

target_link_libraries(PDFWriterTestPlayground pdfwriter) #add_dependencies makes sure that dependencies are built before main target

add_custom_target(check
    ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/TestMaterials ${CMAKE_BINARY_DIR}/Testing/TestMaterials
    COMMAND $<TARGET_FILE:PDFWriterTestPlayground> -b ${CMAKE_BINARY_DIR}/Testing
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS PDFWriterTestPlayground
)
