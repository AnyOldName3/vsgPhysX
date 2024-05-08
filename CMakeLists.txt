cmake_minimum_required(VERSION 3.16)

project(vsgPhysX)

set(vsgphysxversion 0.1.0)

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
option(PHYSX_FROM_VCPKG "Use PhysX from vcpkg, which provides CMake config" OFF)
if (NOT PHYSX_FROM_VCPKG)
    find_package(PhysX5 5.0.0 REQUIRED)
    set(physx_namespace PhysX5)
    set(physx_main_library PhysX)
else()
    find_package(unofficial-omniverse-physx-sdk CONFIG REQUIRED)
    set(physx_namespace unofficial::omniverse-physx-sdk)
    set(physx_main_library sdk)
endif()

add_subdirectory(include/vsgPhysX)
add_subdirectory(src/vsgPhysX)

add_library(vsgPhysX ${HEADERS} ${SOURCES})
set_property(TARGET vsgPhysX APPEND PROPERTY PUBLIC_HEADER ${HEADERS})
set_target_properties(vsgPhysX PROPERTIES VERSION ${vsgphysxversion})
add_library(vsgPhysX::vsgPhysX ALIAS vsgPhysX)

target_include_directories(vsgPhysX
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/vsgPhysX>"
)

target_link_libraries(vsgPhysX vsg::vsg ${physx_namespace}::${physx_main_library})

if (BUILD_SHARED_LIBS)
    target_compile_definitions(vsgPhysX INTERFACE VSGPHYSX_SHARED_LIBRARY)
endif()

include(GNUInstallDirs)
install(TARGETS vsgPhysX
    EXPORT vsgPhysXTargets
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vsgPhysX
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/$<CONFIG>
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/$<CONFIG>
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/$<CONFIG>
)
install(EXPORT vsgPhysXTargets
    NAMESPACE vsgPhysX::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vsgPhysX
)

include(CMakePackageConfigHelpers)
configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/vsgPhysXConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/vsgPhysX"
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/vsgPhysXConfigVersion.cmake"
    VERSION ${vsgphysxversion}
    COMPATIBILITY AnyNewerVersion
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/vsgPhysXConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/vsgPhysXConfigVersion.cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindPhysX5.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/vsgPhysX"
)

export(EXPORT vsgPhysXTargets FILE "${CMAKE_CURRENT_BINARY_DIR}/vsgPhysXTargets.cmake")

option(VSGPHYSX_BUILD_EXAMPLES "Build vsgPhysX examples" ON)
if (VSGPHYSX_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
