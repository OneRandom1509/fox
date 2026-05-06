cmake_minimum_required(VERSION 3.25)

project(
    fox
    VERSION 0.1
    LANGUAGES CXX
)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
endif()

# ------------------------------------------------
# Compiler settings
# ------------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------
# Sources
# ------------------------------------------------

set(FOX_SOURCES
    src/main.cpp
    src/Executor.cpp
    src/Token.cpp
    src/Scanner.cpp
    #    src/AstPrinter.cpp
    src/Parser.cpp
    src/RuntimeError.cpp
    src/Interpreter.cpp
    src/Environment.cpp
    src/callable/FoxFunction.cpp
    src/lib/clock.cpp
)

# ------------------------------------------------
# Executable
# ------------------------------------------------

add_executable(fox ${FOX_SOURCES})

target_include_directories(fox
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/ast
        ${PROJECT_SOURCE_DIR}/include/lib
        ${PROJECT_SOURCE_DIR}/include/callable
)

# ------------------------------------------------
# Warnings
# ------------------------------------------------

target_compile_options(fox
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
)

# ------------------------------------------------
# Optional: ccache support
# ------------------------------------------------

find_program(CCACHE_PROGRAM ccache)

if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()
