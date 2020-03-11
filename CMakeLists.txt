cmake_minimum_required(VERSION 3.7.0)
project(rocketsd)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set( SOURCES
    shared/interfaces/radio/radio_packet.c
    src/app.cc
    src/log.cc
    src/main.cc
    src/modules/curses/curse_logstream.cc
    src/modules/curses/curse_module.cc
    src/modules/curses/curse_window.cc
    src/modules/cute_module.cc
    src/modules/fake_module.cc
    src/modules/influx_module.cc
    src/modules/module.cc
    src/modules/module_factory.cc
    src/modules/serial_module.cc
    src/protocol_parser.cc
    src/protocol.cc
    src/util.cc
)

SET( HEADERS
    inc/app.h
    inc/log.h
    src/modules/curses/curse_logstream.h
    src/modules/curses/curse_window.h
    inc/modules/curse_module.h
    inc/modules/cute_module.h
    inc/modules/fake_module.h
    inc/modules/influx_module.h
    inc/modules/module_factory.h
    inc/modules/module.h
    inc/modules/serial_module.h
    inc/proto/packet.h
    inc/protocol_parser.h
    inc/protocol.h
    inc/util.h
    shared/interfaces/id.h
    shared/interfaces/radio/radio_packet.h
)

SET( PROTOS
    proto/packet.proto
)

# Qt5
set (CMAKE_PREFIX_PATH /opt/Qt/5.14.1/gcc_64/)
find_package(Qt5 COMPONENTS Core Network Xml SerialPort REQUIRED)

# Protobuf, for messages
find_package(Protobuf REQUIRED)

protobuf_generate_cpp(PROTO_SOURCES PROTO_HEADERS ${PROTOS})

set_property(SOURCE ${PROTO_HEADERS} PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE ${PROTO_SOURCES} PROPERTY SKIP_AUTOGEN ON)

# Nlohmann JSON, for config
find_package(nlohmann_json 3.7.0 REQUIRED)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(rocketsd ${SOURCES} ${HEADERS} ${PROTO_HEADERS} ${PROTO_SOURCES})
set_property(TARGET rocketsd PROPERTY CXX_STANDARD 17)
target_link_libraries(rocketsd Qt5::Network Qt5::Xml Qt5::SerialPort protobuf::libprotobuf gcov nlohmann_json::nlohmann_json ${CURSES_LIBRARY})
target_include_directories(rocketsd PRIVATE . inc ${PROTOBUF_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
target_compile_options(rocketsd PRIVATE -Wall -Wpedantic)
