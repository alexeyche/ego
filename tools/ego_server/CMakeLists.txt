cmake_minimum_required(VERSION 2.8)

# Define the files we need to compile
# Anything not in this list will not be compiled into the output library
# Do not include test programs here
set(SOURCES
)

# add directory name to sources
set(DIR_SRCS)
foreach(file ${SOURCES})
  set(DIR_SRCS ${DIR_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/${file})
endforeach()
# append sources (with directory name) to list of all EGO sources (used at the parent scope)
set(EGO_SRCS ${EGO_SRCS} ${DIR_SRCS} PARENT_SCOPE)

# executable
add_executable(ego-server
    ego_server_main.cpp
)


include_directories(${CMAKE_SOURCE_DIR}/ego)
#include_directories(${CMAKE_BINARY_DIR}/protos)
# link dependencies of executable
target_link_libraries(ego-server
	${PROTOBUF_LIBRARY}
    ego
    m
)
install(TARGETS ego-server RUNTIME DESTINATION bin)
install(DIRECTORY static DESTINATION ego-server)
install(CODE
    "EXECUTE_PROCESS(COMMAND ln -sf ${CMAKE_INSTALL_PREFIX}/bin/ego-server ${CMAKE_INSTALL_PREFIX}/ego-server/static)"
)