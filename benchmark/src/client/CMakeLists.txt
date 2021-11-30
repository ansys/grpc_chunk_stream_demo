add_executable(sync_client sync_client.cpp)
target_include_directories(sync_client PUBLIC ${CONAN_LIB_DIRS})

target_link_libraries(sync_client ${CONAN_LIBS})
target_link_libraries(sync_client send_array)
target_link_libraries(sync_client common)

add_executable(measure_memcpy measure_memcpy.cpp)
