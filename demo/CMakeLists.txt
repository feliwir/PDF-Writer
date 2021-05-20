add_executable(PDFWriterTestPlayground 

#sources
FileURL.cpp
PosixPath.cpp
TestsRunner.cpp
TextUsageBugs.cpp
TiffSpecialsTest.cpp
WindowsPath.cpp
PDFWriterTestPlayground.cpp

#headers
FileURL.h
ITestUnit.h
PosixPath.h
TestsRunner.h
TextUsageBugs.h
TiffSpecialsTest.h
)

target_link_libraries(PDFWriterTestPlayground pdfwriter) #add_dependencies makes sure that dependencies are built before main target

add_custom_target(check
    ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/TestMaterials ${CMAKE_BINARY_DIR}/Testing/TestMaterials
    COMMAND $<TARGET_FILE:PDFWriterTestPlayground> -b ${CMAKE_BINARY_DIR}/Testing
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS PDFWriterTestPlayground
)
