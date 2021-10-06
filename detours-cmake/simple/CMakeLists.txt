add_executable(simple simple.cpp)

target_link_libraries(simple PRIVATE lib_detours ws2_32)