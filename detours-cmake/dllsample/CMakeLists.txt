# The hook library
add_library(dllsample SHARED dllsample.cpp)
target_link_libraries(dllsample PRIVATE lib_detours ws2_32)
set_target_properties(dllsample PROPERTIES LINK_FLAGS /EXPORT:DetourFinishHelperProcess,@1,NONAME)

# Dummy executable
add_definitions(-D_AFXDLL) 
set(CMAKE_MFC_FLAG 1) 
FILE(GLOB SRC_FILES "./dummy/*.cpp")
FILE(GLOB HEAD_FILES "./dummy/*.h")
FILE(GLOB RC_FILES "./dummy/*.rc")
add_executable(dummy ${SRC_FILES} ${HEAD_FILES} ${RC_FILES})
set_target_properties(dummy PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")

# With dll that runs the dummy executable with the dllsample (hook) injected
add_executable(runwithdll runwithdll.cpp)
target_link_libraries(runwithdll lib_detours)
