cmake_minimum_required(VERSION 3.11)
project(ImStyleEd-tests)

# ---Create executable---
add_executable(${PROJECT_NAME} tests.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# ---Set warning level---
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -pedantic-errors -Wconversion -Wsign-conversion)
endif()

# ---Maybe enable warnings as errors---
set(WARNINGS_AS_ERRORS_FOR_IMSTYLEED OFF CACHE BOOL "ON iff you want to treat warnings as errors")
if(WARNINGS_AS_ERRORS_FOR_IMSTYLEED)
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /WX)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
    endif()
endif()

# ---Set executable output directory---
get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PARENT_DIR}/bin/${CMAKE_BUILD_TYPE})

# ---Include our library---
add_subdirectory(.. ${CMAKE_CURRENT_SOURCE_DIR}/build/ImStyleEd)
target_link_libraries(${PROJECT_NAME} PRIVATE ImStyleEd::ImStyleEd)

# ---Add exe_path---
include(FetchContent)
FetchContent_Declare(
    exe_path
    GIT_REPOSITORY https://github.com/coolLibs/exe_path
    GIT_TAG bcf81c8e1d48c056081e232fe297c73743e72420
)
FetchContent_MakeAvailable(exe_path)
target_link_libraries(${PROJECT_NAME} PRIVATE exe_path::exe_path)

# ---Add doctest---
include(FetchContent)
FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest
    GIT_TAG b7c21ec5ceeadb4951b00396fc1e4642dd347e5f
)
FetchContent_MakeAvailable(doctest)
target_link_libraries(${PROJECT_NAME} PRIVATE doctest::doctest)

# ---Add quick_imgui---
include(FetchContent)
FetchContent_Declare(
    quick_imgui
    GIT_REPOSITORY https://github.com/CoolLibs/quick_imgui
    GIT_TAG 612b6afe1908ad05d26efa344e18cb7a05791843
)
FetchContent_MakeAvailable(quick_imgui)
target_include_directories(ImStyleEd PRIVATE ${quick_imgui_SOURCE_DIR}/lib) # Give our library access to Dear ImGui
target_link_libraries(${PROJECT_NAME} PRIVATE quick_imgui::quick_imgui)

# ---Ignore .vscode/settings.json in Git---
find_package(Git QUIET)
if(GIT_FOUND)
    get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
    if (EXISTS "${PARENT_DIR}/.git")
        execute_process(COMMAND ${GIT_EXECUTABLE} update-index --assume-unchanged .vscode/settings.json
            WORKING_DIRECTORY ${PARENT_DIR}
            RESULT_VARIABLE ERRORS)
        if(NOT ERRORS EQUAL "0")
            message("Git assume-unchanged failed: ${ERRORS}")
        endif()
    else()
        message("No Git repository found.")
    endif()
else()
    message("Git executable not found.")
endif()
