cmake_minimum_required(VERSION 3.1)

project(main)

SET(CMAKE_BUILD_TYPE Release)

SET (CMAKE_CXX_STANDARD 11)

SET (CMAKE_CXX_FLAGS "")
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -g0")
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -mtune=native") # INTEL CPUs
#SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=native -mtune=native") # ARM CPUs
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

SET (CMAKE_EXE_LINKER_FLAGS "-lm")

# Generate the source files list
file (GLOB_RECURSE source_files src/*)

# Add includes
include_directories (src)

# Object
add_library (dec-obj OBJECT ${source_files})

add_executable(main $<TARGET_OBJECTS:dec-obj>)
