add_executable(pdfmerge pdfmerge.cpp)

target_link_libraries(pdfmerge PRIVATE libcharta)
target_include_directories(pdfmerge PRIVATE ${CMAKE_SOURCE_DIR}/contrib/cxxopts/include)

# Installing
if(NOT SKIP_INSTALL_ALL )
    install(TARGETS pdfmerge   
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} 
        FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()