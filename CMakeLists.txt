cmake_minimum_required(VERSION 3.10)

# needed for export of compile commands for clangd and clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(nbody
    VERSION 1.0
    LANGUAGES CXX C
)

# find clang-tidy and enable if available
# this will automatically run clang-tidy on each compilation
# find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
# if(CLANG_TIDY_EXE)
#     set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
# endif()

# find and enable OpenMP if available
find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif ()

# get doctest as testing framework
include(FetchContent)
FetchContent_Declare(
    doctest
    URL https://github.com/doctest/doctest/archive/refs/tags/v2.4.12.zip
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)
FetchContent_MakeAvailable(doctest)

# get CLI11 as command line parser
FetchContent_Declare(
    CLI11
    URL https://github.com/CLIUtils/CLI11/archive/refs/tags/v2.6.1.zip
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)
FetchContent_MakeAvailable(CLI11)

# if supported, use faster compilation flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-O3 -march=native -flto)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/O2 /GL)
endif()

# we will target modern c++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# create a static library for core functionality
# we will link this to the main executable and tests later
file(GLOB SOURCES_CORE CONFIGURE_DEPENDS 
    "src/common/*.cpp"
    "src/integrator/*.cpp"
    "src/scenario/*.cpp"
    "src/space/*.cpp"
)

add_library(nbody_core STATIC ${SOURCES_CORE})

target_include_directories(nbody_core PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# find cli sources
file(GLOB SOURCES_CLI CONFIGURE_DEPENDS 
    "src/cli/*.cpp"
)
# create the main executable for the nbody simulator
add_executable(nbody ${SOURCES_CLI})
# link core library to main executable
target_link_libraries(nbody PRIVATE nbody_core CLI11::CLI11)

# enable testing module
enable_testing()

# add tests
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "tests/*.cpp")
add_executable(nbody_test ${TEST_SOURCES})
# link core library to tests
target_link_libraries(nbody_test PRIVATE
    nbody_core doctest::doctest
)
# add include directories for tests
target_include_directories(nbody_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/tests"
    ${doctest_SOURCE_DIR}/doctest
)
# add test to ctest
add_test(NAME nbody_test COMMAND nbody_test)

# enable automatic test discovery for doctest
include("${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake")
doctest_discover_tests(nbody_test)