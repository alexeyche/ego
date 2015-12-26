# If the pyx file is a C++ file, we should specify that here.

include_directories(${PROJECT_SOURCE_DIR}/ego)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/egopy.pyx 
    ${CMAKE_CURRENT_SOURCE_DIR}/ego_wrapper.cxx 
    PROPERTIES CYTHON_IS_CXX TRUE 
)


cython_add_module( egopy egopy.pyx ego_wrapper.cxx )

target_link_libraries(egopy ego)

install(TARGETS egopy  DESTINATION lib/ COMPONENT)