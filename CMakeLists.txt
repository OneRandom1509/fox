cmake_minimum_required(VERSION 3.25)

project(
    fox
    VERSION 0.1
    LANGUAGES CXX
)

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
    src/AstPrinter.cpp
    src/Parser.cpp
)

# ------------------------------------------------
# Executable
# ------------------------------------------------

add_executable(fox ${FOX_SOURCES})

target_include_directories(fox
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
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
