# AES
add_library(aes 
${CMAKE_CURRENT_SOURCE_DIR}/aes/aes_modes.c
${CMAKE_CURRENT_SOURCE_DIR}/aes/aes_ni.c
${CMAKE_CURRENT_SOURCE_DIR}/aes/aesaux.c
${CMAKE_CURRENT_SOURCE_DIR}/aes/aescrypt.c
${CMAKE_CURRENT_SOURCE_DIR}/aes/aeskey.c
${CMAKE_CURRENT_SOURCE_DIR}/aes/aeskey2.c
${CMAKE_CURRENT_SOURCE_DIR}/aes/aestab.c)

target_include_directories(aes PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/aes/)

# Googletest
set(BUILD_GMOCK OFF)
set(INSTALL_GTEST OFF)
add_subdirectory(googletest)