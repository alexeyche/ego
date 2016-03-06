cmake_minimum_required(VERSION 2.8)
project(ego C CXX)

set(EGO_MAJOR_VERSION 0)
set(EGO_MINOR_VERSION 1)
set(EGO_PATCH_VERSION 0)
set(EGO_VERSION
  ${EGO_MAJOR_VERSION}.${EGO_MINOR_VERSION}.${EGO_PATCH_VERSION})

# First, define all the compilation options.
# We default to debugging mode for developers.
option(DEBUG "Compile with debugging information" OFF)
option(PROFILE "Compile with profiling information" OFF)

# Set the CFLAGS and CXXFLAGS depending on the options the user specified.
# Only GCC-like compilers support -Wextra, and other compilers give tons of
# output for -Wall, so only -Wall and -Wextra on GCC.
#if(CMAKE_COMPILER_IS_GNUCC)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
#  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
#endif(CMAKE_COMPILER_IS_GNUCC)
include(FindProtobuf)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake )

find_package(Protobuf REQUIRED)
find_package(Armadillo REQUIRED)
find_package(Threads REQUIRED)
find_library(OpenBLAS_LIBRARY openblas /opt/OpenBLAS/lib)


# Debugging CFLAGS.  Turn optimizations off; turn debugging symbols on.
if(DEBUG)
  add_definitions(-DDEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O0")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -g -O0")
elseif(PROFILE)
  add_definitions(-DNDEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pg")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -pg")
else()
  add_definitions(-DNDEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -std=c++11 -O3")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffast-math -std=gnu99 -O3")
endif(DEBUG)

if(PERF)
  add_definitions(-DPERF)
endif(PERF)

# Profiling CFLAGS.  Turn profiling information on.
#if(PROFILE)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
#  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
#  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
#endif(PROFILE)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})

include(UseCython)


# Recurse into the rest of the project.
add_subdirectory(ego)
#add_subdirectory(python)
add_subdirectory(tools)
