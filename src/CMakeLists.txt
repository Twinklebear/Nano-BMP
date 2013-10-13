set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O2 -Wall -Wextra -pedantic -DBUILDING_NANO")
add_library(nano_bmp SHARED nano_bmp.c)
install(TARGETS nano_bmp DESTINATION "${Nano_BMP_SOURCE_DIR}/bin")
