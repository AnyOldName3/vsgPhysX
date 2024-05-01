cmake_minimum_required(VERSION 3.16)

project(vsgPhysX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# set the use of C++17 globally as all examples require it
set(CMAKE_CXX_STANDARD 17)

#if (MSVC)
#    option(OVERRIDE_MSVC_RUNTIME_LIBRARY "Explicitly set the MSVC runtime library instead of allowing CMake to pick. Useful if dependencies aren't compatible with CMake's default" ON)
#    if (OVERRIDE_MSVC_RUNTIME_LIBRARY)
#        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "Select the MSVC runtime library for use by compilers targeting the MSVC ABI.")
#    endif()
#endif()

find_package(vsg REQUIRED)
# TODO: support vcpkg's modified PhysX
find_package(PhysX5 5.0.0 REQUIRED)

add_subdirectory(include/vsgPhysX)
add_subdirectory(src/vsgPhysX)

add_library(vsgPhysX ${HEADERS} ${SOURCES})
add_library(vsgPhysX::vsgPhysX ALIAS vsgPhysX)

target_include_directories(vsgPhysX
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
    PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/vsgPhysX>"
)

target_link_libraries(vsgPhysX vsg::vsg PhysX5::PhysX)

if (BUILD_SHARED_LIBS)
    target_compile_definitions(vsgPhysX INTERFACE VSGPHYSX_SHARED_LIBRARY)
endif()

option(VSGPHYSX_BUILD_EXAMPLES "Build vsgPhysX examples" ON)
if (VSGPHYSX_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
