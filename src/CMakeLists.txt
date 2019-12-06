AUX_SOURCE_DIRECTORY(. src_files)

ADD_EXECUTABLE(fuelgauge-drv ${src_files})

find_package(Threads)

target_link_libraries(fuelgauge-drv ${CMAKE_THREAD_LIBS_INIT})

INSTALL(TARGETS fuelgauge-drv
	RUNTIME DESTINATION bin
)


