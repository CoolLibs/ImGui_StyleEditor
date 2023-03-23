cmake_minimum_required(VERSION 3.8)

set(WARNINGS_AS_ERRORS_FOR_IMSTYLEED OFF CACHE BOOL "ON iff you want to treat warnings as errors")

add_library(ImStyleEd)
add_library(ImStyleEd::ImStyleEd ALIAS ImStyleEd)
target_compile_features(ImStyleEd PUBLIC cxx_std_17)

# ---Add source files---
if(WARNINGS_AS_ERRORS_FOR_IMSTYLEED)
    target_include_directories(ImStyleEd PUBLIC include)
else()
    target_include_directories(ImStyleEd SYSTEM PUBLIC include)
endif()
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)
target_sources(ImStyleEd PRIVATE ${SRC_FILES})

# ---Set warning level---
if(MSVC)
    target_compile_options(ImStyleEd PRIVATE /W4)
else()
    target_compile_options(ImStyleEd PRIVATE -Wall -Wextra -Wpedantic -pedantic-errors)
endif()

# ---Maybe enable warnings as errors---
if(WARNINGS_AS_ERRORS_FOR_IMSTYLEED)
    if(MSVC)
        target_compile_options(ImStyleEd PRIVATE /WX)
    else()
        target_compile_options(ImStyleEd PRIVATE -Werror)
    endif()
endif()

# ---Cereal library---
include(FetchContent)
FetchContent_Declare(
    cereal
    GIT_REPOSITORY https://github.com/USCiLab/cereal
    GIT_TAG ddd467244713ea4fe63733628992efcdd6a9187d
)
FetchContent_Populate(cereal)
target_include_directories(ImStyleEd PUBLIC ${cereal_SOURCE_DIR}/include)

# ---glm library---
include(FetchContent)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/CoolLibs/glm
    GIT_TAG 41628991f848835d36042173eb45874c1859e1c2
)
FetchContent_Populate(glm)
target_include_directories(ImStyleEd PRIVATE ${glm_SOURCE_DIR})