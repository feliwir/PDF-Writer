
add_library(aes 
${CMAKE_CURRENT_SOURCE_DIR}/aes/aes_avs.c
${CMAKE_CURRENT_SOURCE_DIR}/aes/aes_modes.c
${CMAKE_CURRENT_SOURCE_DIR}/aes/aes_ni.c
${CMAKE_CURRENT_SOURCE_DIR}/aes/aescrypt.c
${CMAKE_CURRENT_SOURCE_DIR}/aes/aesgav.c
${CMAKE_CURRENT_SOURCE_DIR}/aes/aeskey.c
${CMAKE_CURRENT_SOURCE_DIR}/aes/aeskey2.c
${CMAKE_CURRENT_SOURCE_DIR}/aes/aesrav.c
${CMAKE_CURRENT_SOURCE_DIR}/aes/aestab.c)

target_include_directories(aes PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/aes/)