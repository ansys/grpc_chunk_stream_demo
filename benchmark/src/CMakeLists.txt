add_executable(sync_server sync_server.cpp)
target_include_directories(sync_server PUBLIC ${CONAN_LIB_DIRS})

target_link_libraries(sync_server ${CONAN_LIBS})
target_link_libraries(sync_server send_array)

add_executable(sync_client sync_client.cpp)
target_include_directories(sync_client PUBLIC ${CONAN_LIB_DIRS})

target_link_libraries(sync_client ${CONAN_LIBS})
target_link_libraries(sync_client send_array)
