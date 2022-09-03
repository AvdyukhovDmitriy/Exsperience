cmake_minimum_required(VERSION 3.10)

project(Transport_Catalogue CXX)
set(CMAKE_CXX_STANDARD 17)


find_package(Protobuf REQUIRED)

find_package(Threads REQUIRED)


protobuf_generate_cpp(TRANSPORT_CATALOGUE_PROTO_SRCS TRANSPORT_CATALOGUE_PROTO_HDRS src/transport_catalogue.proto
                                                                                    src/map_renderer.proto
                                                                                    src/svg.proto
                                                                                    src/graph.proto
                                                                                    src/transport_router.proto)

set(TRANSPORT_CATALOGUE_FILES src/main.cpp 
                              src/domain.h
                              src/geo.cpp src/geo.h
                              src/graph.h
                              src/json_builder.cpp src/json_builder.h
                              src/json_reader.cpp src/json_reader.h
                              src/json.cpp src/json.h
                              src/map_renderer.cpp src/map_renderer.h
                              src/ranges.h
                              src/request_handler.cpp src/request_handler.h
                              src/router.h
                              src/serialization.cpp src/serialization.h
                              src/svg.cpp src/svg.h
                              src/transport_catalogue.cpp src/transport_catalogue.h
                              src/transport_catalogue.proto
                              src/transport_router.cpp src/transport_router.h)

add_executable(transport_catalogue ${TRANSPORT_CATALOGUE_PROTO_SRCS} ${TRANSPORT_CATALOGUE_PROTO_HDRS} ${TRANSPORT_CATALOGUE_FILES})

target_include_directories(transport_catalogue PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(transport_catalogue PUBLIC ${CMAKE_CURRENT_BINARY_DIR})


string(REPLACE "protobuf.lib" "protobufd.lib" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
string(REPLACE "protobuf.a" "protobufd.a" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
target_link_libraries(transport_catalogue "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}>" Threads::Threads)