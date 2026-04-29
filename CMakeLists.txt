cmake_minimum_required(VERSION 3.17)
project(TheProjectWorkspace LANGUAGES CXX)

# Global Modern C++ standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure executable can find shared library at runtime (Linux/macOS)
set(CMAKE_RPATH_USE_LINK_PATH TRUE)

# Redirect all library files (thengine.dll / .so)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Redirect all executable files (thesandbox.exe)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Redirect archive files (static libs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 1. Find SDL3 (Required by your rules)
find_package(SDL3 REQUIRED CONFIG COMPONENTS SDL3-shared)

# 2. Add the engine library
add_subdirectory(engine)

# 3. Add the sandbox application
add_subdirectory(sandbox)
