file(GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")


set(PROTOBUF_IMPORT_DIRS ${PROTOBUF_INCLUDE_DIRS})

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${ProtoFiles})

set(ProtobufIncludePath ${PROTOBUF_INCLUDE_DIRS}
        CACHE INTERNAL "Path to generated protobuf files.")

add_library(ego_protos ${ProtoFiles} ${PROTO_SRCS} ${PROTO_HDRS})

SET_TARGET_PROPERTIES(ego_protos PROPERTIES COMPILE_FLAGS "-fPIC")

target_link_libraries(ego_protos
    ${PROTOBUF_LIBRARY}
)
