cmake_minimum_required(VERSION 2.8)

set(DIRS
    ego
)

foreach(dir ${DIRS})
    add_subdirectory(${dir})
endforeach()

set(EGO_SRCS ${EGO_SRCS} ${DIR_SRCS} PARENT_SCOPE)
