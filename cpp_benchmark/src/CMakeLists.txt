add_executable(timer timer.cpp)
target_include_directories(timer PUBLIC ${CONAN_LIB_DIRS})
target_link_libraries(timer ${CONAN_LIBS})
