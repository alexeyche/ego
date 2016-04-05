cmake_minimum_required(VERSION 2.8)

# Define the files we need to compile
# Anything not in this list will not be compiled into the output library
# Do not include test programs here
set(SOURCES
	tests.cpp
	register.cpp
  util_tests.cpp
  test_framework.cpp
)

# add directory name to sources
set(DIR_SRCS)
foreach(file ${SOURCES})
  set(DIR_SRCS ${DIR_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/${file})
endforeach()
# append sources (with directory name) to list of all EGO sources (used at the parent scope)
set(EGO_SRCS ${EGO_SRCS} ${DIR_SRCS} PARENT_SCOPE)

# executable
add_executable(ego-test
    main.cpp
    ${SOURCES}
)

include_directories(${CMAKE_SOURCE_DIR}/ego)
#include_directories(${CMAKE_BINARY_DIR}/protos)
# link dependencies of executable
target_link_libraries(ego-test
	${PROTOBUF_LIBRARY}
    ego
    m
)
install(TARGETS RUNTIME DESTINATION bin)

add_custom_command(
  OUTPUT ego-test.pass
  POST_BUILD
  COMMAND ego-test && touch ego-test.pass
  COMMENT "Running test"
  DEPENDS ego-test
)

add_custom_target(
    check_test ALL
    DEPENDS ego-test.pass
)


