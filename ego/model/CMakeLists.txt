cmake_minimum_required(VERSION 2.8)

set(SOURCES
	base_model.cpp
	config.cpp
    model.cpp
    tree_model.cpp
)

set(DIR_SRCS)
foreach(file ${SOURCES})
  set(DIR_SRCS ${DIR_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/${file})
endforeach()
set(EGO_SRCS ${EGO_SRCS} ${DIR_SRCS} PARENT_SCOPE)
