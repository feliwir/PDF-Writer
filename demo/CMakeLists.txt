add_executable(PDFWriterTestPlayground 

#sources
RotatedPagesPDF.cpp
FileURL.cpp
PDFObjectCastTest.cpp
PosixPath.cpp
RecryptPDF.cpp
RefCountTest.cpp
ShutDownRestartTest.cpp
TestMeasurementsTest.cpp
TestsRunner.cpp
TextUsageBugs.cpp
TiffSpecialsTest.cpp
TimerTest.cpp
UppercaseSequanceTest.cpp
WindowsPath.cpp
PDFWriterTestPlayground.cpp
UnicodeTextUsage.cpp

#headers
RotatedPagesPDF.h
FileURL.h
ITestUnit.h
PDFObjectCastTest.h
PosixPath.h
RecryptPDF.h
RefCountTest.h
ShutDownRestartTest.h
TestMeasurementsTest.h
TestsRunner.h
TextUsageBugs.h
TiffSpecialsTest.h
TimerTest.h
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
