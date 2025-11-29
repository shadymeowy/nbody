cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(nbody
        VERSION 1.0
        LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(
        ${CMAKE_SOURCE_DIR}/src/common
        ${CMAKE_SOURCE_DIR}/src/BarnesHut
        ${CMAKE_SOURCE_DIR}/src/Octree
)

find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif ()

add_executable(${PROJECT_NAME}
        "src/main.cpp"
        src/body.cpp
        src/Octree/Octree.cpp
        src/Octree/get_octant.cpp
        src/Octree/SubdivideRecursively.cpp
        src/Octree/InitLeafOctants.cpp
        src/BarnesHut/CenterOfMass.cpp
        src/BarnesHut/BarnesHutApproximation.cpp
        src/BarnesHut/CalculateForces.cpp
        src/Common/Octan.h
        src/Common/body.h
)

target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
