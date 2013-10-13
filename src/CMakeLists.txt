set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O2 -Wall -Wextra -pedantic -DBUILDING_NANO")
add_library(nano_bmp SHARED nano_bmp.c)

IF (NOT ${WIN32})
	install(FILES "${Nano_BMP_SOURCE_DIR}/include/nano_bmp.h" DESTINATION include)
	install(TARGETS nano_bmp
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
	)
ELSE(NOT ${WIN32})
	install(TARGETS nano_bmp DESTINATION "${Nano_BMP_SOURCE_DIR}/bin")
ENDIF(NOT ${WIN32})

