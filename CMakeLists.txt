cmake_minimum_required(VERSION 3.8)

add_library(ImStyleEd)
add_library(ImStyleEd::ImStyleEd ALIAS ImStyleEd)
target_compile_features(ImStyleEd PUBLIC cxx_std_11)

# ---Add source files---
target_include_directories(ImStyleEd PUBLIC include)
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)
target_sources(ImStyleEd PRIVATE ${SRC_FILES})

# ---Set warning level---
if(MSVC)
    target_compile_options(ImStyleEd PRIVATE /W4)
else()
    target_compile_options(ImStyleEd PRIVATE -Wall -Wextra -Wpedantic -pedantic-errors -Wconversion -Wsign-conversion)
endif()

# ---Maybe enable warnings as errors---
set(WARNINGS_AS_ERRORS_FOR_IMSTYLEED OFF CACHE BOOL "ON iff you want to treat warnings as errors")
if(WARNINGS_AS_ERRORS_FOR_IMSTYLEED)
    if(MSVC)
        target_compile_options(ImStyleEd PRIVATE /WX)
    else()
        target_compile_options(ImStyleEd PRIVATE -Werror)
    endif()
endif()
