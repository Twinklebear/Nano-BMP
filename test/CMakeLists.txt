set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -pedantic")
add_executable(nano_test test.c)
target_link_libraries(nano_test nano_bmp)
install(TARGETS nano_test DESTINATION "${Nano_BMP_SOURCE_DIR}/bin")
