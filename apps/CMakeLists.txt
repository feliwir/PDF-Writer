add_executable(pdfmerge pdfmerge.cpp)

target_link_libraries(pdfmerge PRIVATE libcharta)
target_include_directories(pdfmerge PRIVATE ${CMAKE_SOURCE_DIR}/contrib/cxxopts/include)