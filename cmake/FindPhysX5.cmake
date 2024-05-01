# Distributed under a mystery licence to be determined later.

#[=======================================================================[.rst:
FindPhysX5
-------

TODO: this documentation comment is largely bogus

Finds the PhysX5 library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``PhysX5::PhysX``
    The main PhysX library and its mandatory dependencies

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``PhysX5_FOUND``
    True if the system has the PhysX5 library.
``PhysX5_VERSION``
    The version of the PhysX5 library which was found.
``PhysX5_INCLUDE_DIRS``
    Include directories needed to use PhysX5.
``PhysX5_LIBRARIES``
    Libraries needed to link to PhysX5.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``PhysX5_INCLUDE_DIR``
    The directory containing ``PxPhysics.h``.
``PhysX5_LIBRARY``
    The path to the PhysX5 library.

#]=======================================================================]

set(PHYSX_BUILD_TYPE "checked" CACHE STRING "The build type of PhysX used when PHYSX_MAP_IMPORTED_CONFIG_<config> is unset (except Debug, which defaults to debug for convenience), i.e., one of {debug, checked, profile, release}")
set(PHYSX_PLATFORM_DIRECTORY_NAME "*" CACHE STRING "The platform-specific directory PhysX put itself in, e.g. win.x86_64.vc143.md")

foreach (config ${CMAKE_CONFIGURATION_TYPES})
    if (NOT PHYSX_MAP_IMPORTED_CONFIG_${config})
        if (config STREQUAL Debug)
            set(PHYSX_MAP_IMPORTED_CONFIG_${config} Debug)
        else()
            set(PHYSX_MAP_IMPORTED_CONFIG_${config} ${PHYSX_BUILD_TYPE})
        endif()
    endif()
endforeach()

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BITNESS_SUFFIX "64")
else()
    set(BITNESS_SUFFIX "32")
endif()

find_path(PhysX5_INCLUDE_DIR
    NAMES PxPhysics.h
    PATH_SUFFIXES PhysX PhysX/include include
)
if (PhysX5_INCLUDE_DIR)
    file(STRINGS "${PhysX5_INCLUDE_DIR}/foundation/PxPhysicsVersion.h" version_file)
    foreach (part MAJOR MINOR BUGFIX)
        string(REGEX MATCH "#define[ \t\r\n]+PX_PHYSICS_VERSION_${part}[ \t\r\n]+([0123456789]+)" line ${version_file})
        set(PhysX5_VERSION_${part} ${CMAKE_MATCH_1})
    endforeach()
    set(PhysX5_VERSION ${PhysX5_VERSION_MAJOR}.${PhysX5_VERSION_MINOR}.${PhysX5_VERSION_BUGFIX} CACHE STRING "PhysX5 version")
endif()

include(SelectLibraryConfigurations)

macro(find_physx5_library library)
    foreach (config debug checked profile release)
        string(TOUPPER ${config} config_uc)
        find_library(PhysX5_${library}_LIBRARY_${config_uc}
            NAMES ${library} ${library}_${BITNESS_SUFFIX}
            PATH_SUFFIXES "bin/${PHYSX_PLATFORM_DIRECTORY_NAME}/${config}"
        )
        if (NOT PhysX5_${library}_LIBRARY_${config_uc})
            find_library(PhysX5_${library}_LIBRARY_${config_uc}
                NAMES ${library}_static ${library}_static_${BITNESS_SUFFIX}
                PATH_SUFFIXES "bin/${PHYSX_PLATFORM_DIRECTORY_NAME}/${config}"
            )
            if (PhysX5_${library}_LIBRARY_${config_uc})
                # cmake doesn't support libraries only being static for certain configurations, so set overall
                set(PhysX5_${library}_STATIC TRUE)
            endif()
        elseif (WIN32)
            string(REGEX REPLACE "(.)(\\.[^.]+)?$" "\\1.dll" dll_path "${PhysX5_${library}_LIBRARY_${config_uc}}")
            if (EXISTS ${dll_path})
                set(PhysX5_${library}_DLL_${config_uc} "${dll_path}" CACHE FILEPATH "Path to a DLL")
            endif()
        endif()
        if (PhysX5_${library}_LIBRARY_${config_uc})
            set(PhysX5_${library}_FOUND TRUE)
            set(PhysX5_${library}_LIBRARY "${PhysX5_${library}_LIBRARY_${config_uc}}")
        endif()
    endforeach()
    #select_library_configurations(PhysX5_${library})
endmacro()

find_physx5_library(PhysX)
find_physx5_library(PhysXCharacterKinematic)
find_physx5_library(PhysXCommon)
find_physx5_library(PhysXCooking)
find_physx5_library(PhysXExtensions)
find_physx5_library(PhysXFoundation)
find_physx5_library(PhysXPvdSDK)
find_physx5_library(PhysXTask)
find_physx5_library(PhysXVehicle)
find_physx5_library(PhysXVehicle2)
find_physx5_library(PVDRuntime)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PhysX5
    FOUND_VAR PhysX5_FOUND
    REQUIRED_VARS
        PhysX5_INCLUDE_DIR
        PhysX5_PhysX_LIBRARY
        PhysX5_PhysXCommon_LIBRARY
        PhysX5_PhysXExtensions_LIBRARY
        PhysX5_PhysXFoundation_LIBRARY
    HANDLE_COMPONENTS
    VERSION_VAR PhysX5_VERSION
)

foreach (library PhysX PhysXCharacterKinematic PhysXCommon PhysXCooking PhysXExtensions PhysXFoundation PhysXPvdSDK PhysXTask PhysXVehicle PhysXVehicle2 PVDRuntime)
    if (PhysX5_${library}_FOUND AND NOT TARGET PhysX5::${library})
        if (PhysX5_${library}_STATIC)
            add_library(PhysX5::${library} STATIC IMPORTED)
        else()
            add_library(PhysX5::${library} SHARED IMPORTED)
        endif()
        set_target_properties(PhysX5::${library} PROPERTIES
            VERSION ${PhysX5_VERSION}
        )
        target_include_directories(PhysX5::${library} INTERFACE "${PhysX5_INCLUDE_DIR}")
        foreach (config debug checked profile release)
            string(TOUPPER ${config} config_uc)
            if (PhysX5_${library}_LIBRARY_${config_uc})
                set_property(TARGET PhysX5::${library} APPEND PROPERTY IMPORTED_CONFIGURATIONS ${config_uc})
                if (WIN32 AND NOT PhysX5_${library}_STATIC)
                    set_target_properties(PhysX5::${library} PROPERTIES
                        IMPORTED_LOCATION_${config_uc} "${PhysX5_${library}_DLL_${config_uc}}"
                        IMPORTED_IMPLIB_${config_uc} "${PhysX5_${library}_LIBRARY_${config_uc}}"
                    )
                else()
                    set_target_properties(PhysX5::${library} PROPERTIES
                        IMPORTED_LOCATION_${config_uc} "${PhysX5_PhysX_LIBRARY_${config_uc}}"
                    )
                endif()
            endif()
        endforeach()
    endif()
endforeach()

if (TARGET PhysX5::PhysX)
    set_target_properties(PhysX5::PhysX PROPERTIES
        INTERFACE_LINK_LIBRARIES "PhysX5::PhysXCommon;PhysX5::PhysXExtensions;PhysX5::PhysXFoundation"
    )
endif()
