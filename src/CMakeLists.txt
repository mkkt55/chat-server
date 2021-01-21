find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

aux_source_directory(. SRCFILES)
add_executable(main ${SRCFILES})

target_link_libraries(main -lprotobuf -lpthread)
