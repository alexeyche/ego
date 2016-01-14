cmake_minimum_required(VERSION 2.8)

# Define the files we need to compile
# Anything not in this list will not be compiled into the output library
# Do not include test programs here
set(SOURCES
	tests.cpp
	register.cpp
)

# add directory name to sources
set(DIR_SRCS)
foreach(file ${SOURCES})
  set(DIR_SRCS ${DIR_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/${file})
endforeach()
# append sources (with directory name) to list of all EGO sources (used at the parent scope)
set(EGO_SRCS ${EGO_SRCS} ${DIR_SRCS} PARENT_SCOPE)

# executable
add_executable(test_ego
    test_main.cpp
    ${SOURCES}
)

include_directories(${CMAKE_SOURCE_DIR}/ego)
#include_directories(${CMAKE_BINARY_DIR}/protos)
# link dependencies of executable
target_link_libraries(test_ego
	${PROTOBUF_LIBRARY}
    ego
    m
)
install(TARGETS RUNTIME DESTINATION bin)

add_custom_command(
  OUTPUT test_ego.pass
  POST_BUILD
  COMMAND test_ego && touch test_ego.pass
  COMMENT "Running test"
  DEPENDS test_ego
)

add_custom_target(
    check_test ALL
    DEPENDS test_ego.pass
)


