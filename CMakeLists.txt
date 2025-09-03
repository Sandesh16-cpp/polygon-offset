cmake_minimum_required(VERSION 3.20)
project(polygon_offset VERSION 1.0.0 LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable modern CMake features
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add compiler flags for optimization and warnings
if(MSVC)
    add_compile_options(/W4 /WX /permissive-)
    add_compile_options(/arch:AVX2)  # Enable SIMD
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    add_compile_options(-march=native)  # Enable SIMD optimizations
endif()

# Header-only library interface
add_library(polygon_offset INTERFACE)
target_include_directories(polygon_offset INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Require C++20 concepts and ranges
target_compile_features(polygon_offset INTERFACE cxx_std_20)

# Testing
option(POLYGON_OFFSET_BUILD_TESTS "Build tests" ON)
if(POLYGON_OFFSET_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
option(POLYGON_OFFSET_BUILD_EXAMPLES "Build examples" ON)
if(POLYGON_OFFSET_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Installation
include(GNUInstallDirs)
install(TARGETS polygon_offset
    EXPORT polygon_offset_targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT polygon_offset_targets
    FILE polygon_offset_targets.cmake
    NAMESPACE polygon_offset::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/polygon_offset
)

# Create config file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/polygon_offset_config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/polygon_offset_config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/polygon_offset
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/polygon_offset_config_version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/polygon_offset_config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/polygon_offset_config_version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/polygon_offset
)