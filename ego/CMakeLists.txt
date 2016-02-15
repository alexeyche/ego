cmake_minimum_required(VERSION 2.8)


# Define the files we need to compile.
# Anything not in this list will not be compiled into EGO.
set(SOURCES

)

# add directory name to sources
set(DIR_SRCS)
foreach(file ${SOURCES})
  set(DIR_SRCS ${DIR_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/${file})
endforeach()
# Append sources (with directory name) to list of all EGO sources (used at
# the parent scope).
set(EGO_SRCS ${DIR_SRCS})

set(DIRS
    acq
    base
    contrib
    cov
    util
    protos
    model
    mean
    lik
    inf
    distr
    opt
    func
    problem
    strategy
)

foreach(dir ${DIRS})
  add_subdirectory(${dir})
endforeach()

add_library(ego SHARED ${EGO_SRCS})

include_directories(${CMAKE_BINARY_DIR}/protos)
include_directories(${ARMADILLO_INCLUDE_DIRS} PARENT_SCOPE)
include_directories(${PROTOBUF_INCLUDE_DIRS} PARENT_SCOPE)
include_directories(${OpenBLAS_INCLUDE_DIRS} PARENT_SCOPE)

SET_TARGET_PROPERTIES(ego PROPERTIES COMPILE_FLAGS "-fPIC")

SET(OpenBLAS_VERSION "0.2.16.dev")
SET(OpenBLAS_INCLUDE_DIRS /opt/OpenBLAS/include)
SET(OpenBLAS_LIBRARIES /opt/OpenBLAS/lib/libopenblas.so)

target_link_libraries(ego
    ego_protos
    nlopt
    ${OpenBLAS_LIBRARIES}
    lapack
    #dlib
    #blas # for dlib
    #${ARMADILLO_LIBRARIES}
    ${PROTOBUF_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    m
)



install(TARGETS ego  DESTINATION lib/ COMPONENT)

install(
    DIRECTORY
    ${PROJECT_SOURCE_DIR}/ego
    DESTINATION
    include
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.hpp"
)
install(
    DIRECTORY
    ${CMAKE_BINARY_DIR}/lib/protos
    DESTINATION
    include/ego/lib
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.hpp"
)
