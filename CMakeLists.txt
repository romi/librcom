cmake_minimum_required(VERSION 3.10)
project(librcom-project)

############################################################

set(CMAKE_MODULES_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)
set(CMAKE_THIRDPARTY_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)

############################################################

include(${CMAKE_MODULES_DIRECTORY}/RomiFunctions.cmake)
set(PI_BUILD FALSE)
set_romi_build_defaults(PI_BUILD)
message("${PROJECT_NAME} PI_BUILD = ${PI_BUILD}")
output_compile_flags()

############################################################

set(${PROJECT_NAME}_VERSION_MAJOR 0)
set(${PROJECT_NAME}_VERSION_MINOR 1)

############################################################

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

############################################################

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(COMMON_COMPILATION_FLAGS "-Wall -Wextra -Wpedantic -Werror")
set(CMAKE_CXX_FLAGS "${COMMON_COMPILATION_FLAGS}")
set(CMAKE_C_FLAGS "${COMMON_COMPILATION_FLAGS}")

set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

############################################################

# Options.
# Build tests by default. We want to run tests whether we are developing or building a release. I mean... why wouldn't you? :)
option(BUILD_TESTS "Build all tests." ON)
option(BUILD_COVERAGE "Build coverage." ON)

if(BUILD_TESTS)

    include(CTest)
    enable_testing()

    if (NOT DEFINED googletest_SOURCE_DIR)
        if (CMAKE_VERSION VERSION_LESS 3.2)
            set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
        else()
            set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
        endif()

        include(${CMAKE_MODULES_DIRECTORY}/DownloadProject.cmake)
        download_project(   PROJ                googletest
                GIT_REPOSITORY      https://github.com/google/googletest.git
                GIT_TAG             v1.12.x
                PREFIX              ${CMAKE_THIRDPARTY_DIRECTORY}/googletest
                ${UPDATE_DISCONNECTED_IF_AVAILABLE}
                )

        # Prevent GoogleTest from overriding our compiler/linker options
        # when building with Visual Studio
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
    endif()

endif() # BUILD_TESTS

# Can't build for coverage without the test.
if (BUILD_COVERAGE AND BUILD_TESTS)
    if (NOT DEFINED lcov_SOURCE_DIR)
        include(${CMAKE_MODULES_DIRECTORY}/CodeCoverage.cmake)

        # lcov doesn't build with CMake so we download, set lcov as an external target, then call make.
        # we then manually add the bin path so that we can call it later.
        download_project(   PROJ              lcov
                URL               https://github.com/linux-test-project/lcov/releases/download/v1.15/lcov-1.15.tar.gz
                PREFIX            ${CMAKE_THIRDPARTY_DIRECTORY}/lcov
                TIMEOUT           180
                ${UPDATE_DISCONNECTED_IF_AVAILABLE}
                )

        set(LCOV_PATH "${lcov_SOURCE_DIR}/bin/lcov")
        set(GENHTML_PATH "${lcov_SOURCE_DIR}/bin/genhtml")
    endif()

    set( COVERAGE_EXCLUDES
            "/usr/*"
            "${CMAKE_SOURCE_DIR}/thirdparty/*"
            "*test/*")

    append_coverage_compiler_flags()

endif()

###########################################################3
include(${CMAKE_MODULES_DIRECTORY}/RomiProjectBuildOptions.cmake)
add_subdirectory(librcom)
add_subdirectory(examples)
add_subdirectory(applications/rcom_registry)
