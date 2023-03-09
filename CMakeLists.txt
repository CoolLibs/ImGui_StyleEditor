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

# ---Cereal library---
include(FetchContent)
FetchContent_Declare(
    cereal
    GIT_REPOSITORY https://github.com/USCiLab/cereal
    GIT_TAG ddd467244713ea4fe63733628992efcdd6a9187d
)
target_include_directories(ImStyleEd PUBLIC ${cereal_SOURCE_DIR}/include)

# ---exe_path library---
include(FetchContent)
FetchContent_Declare(
    exe_path
    GIT_REPOSITORY https://github.com/coolLibs/exe_path
    GIT_TAG bcf81c8e1d48c056081e232fe297c73743e72420
)
FetchContent_MakeAvailable(exe_path)
target_link_libraries(ImStyleEd PRIVATE exe_path::exe_path)

# ---exe_path library---
include(FetchContent)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/CoolLibs/glm
    GIT_TAG 41628991f848835d36042173eb45874c1859e1c2
)
target_include_directories(ImStyleEd PRIVATE ${glm_SOURCE_DIR})