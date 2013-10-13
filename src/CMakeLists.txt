set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O2 -Wall -Wextra -pedantic -DBUILDING_NANO")
add_library(nano_bmp SHARED nano_bmp.c)

IF (${WIN32})
	install(TARGETS nano_bmp DESTINATION "${Nano_BMP_SOURCE_DIR}/bin")
	message(STATUS "windows")
ELSE(${WIN32})
	install(FILES "${Nano_BMP_SOURCE_DIR}/include/nano_bmp.h" DESTINATION include)
	install(TARGETS nano_bmp
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
	)
	message(STATUS "not windows")
ENDIF(${WIN32})

