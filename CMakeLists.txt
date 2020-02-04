cmake_minimum_required(VERSION 3.1.0)
project(rocketsd)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set( SOURCES
    shared/interfaces/radio/radio_packet.c
    src/bullshiter.cc
    src/cute_deserializer.cc
    src/deserializer.cc
    src/main.cc
    src/proto/packet.pb.cc
    src/protocol_parser.cc
    src/protocol.cc
    src/log.cc
    src/app.cc
)

SET( HEADERS
    inc/bullshiter.h
    inc/cute_deserializer.h
    inc/deserializer.h
    inc/proto/packet.h
    inc/proto/packet.pb.h
    inc/protocol_parser.h
    inc/protocol.h
    inc/log.h
    inc/app.h
    shared/interfaces/radio/radio_packet.h
    shared/interfaces/id.h
)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

# Qt5
set (CMAKE_PREFIX_PATH /opt/Qt/5.12.1/gcc_64/)
find_package(Qt5 COMPONENTS Core Network Xml REQUIRED)

# Protobuf, for messages
find_package(Protobuf REQUIRED)

add_executable(rocketsd ${SOURCES} ${HEADERS} ${UI})
set_property(TARGET rocketsd PROPERTY CXX_STANDARD 17)
target_link_libraries(rocketsd Qt5::Network Qt5::Xml protobuf::libprotobuf gcov)
target_include_directories(rocketsd PRIVATE . inc)
target_compile_options(rocketsd PRIVATE -Wall -Wpedantic)
