cmake_minimum_required(VERSION 2.8)

set(DIRS
    log
    server
    serial
    ts
)

foreach(dir ${DIRS})
  add_subdirectory(${dir})
endforeach()

set(SOURCES
    any.cpp
    proto_options.cpp
    string.cpp
    parse_parenthesis.cpp
    fs.cpp
    pretty_print.cpp
    protobuf.cpp
    json.cpp
    sobol.cpp
)

set(DIR_SRCS)
foreach(file ${SOURCES})
  set(DIR_SRCS ${DIR_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/${file})
endforeach()

set(EGO_SRCS ${EGO_SRCS} ${DIR_SRCS} PARENT_SCOPE)
