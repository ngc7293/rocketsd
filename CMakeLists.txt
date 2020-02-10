cmake_minimum_required(VERSION 3.1.0)
project(rocketsd)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set( SOURCES
    shared/interfaces/radio/radio_packet.c
    src/producers/producer.cc
    src/producers/producer_factory.cc
    src/producers/fake_producer.cc
    src/producers/serial_producer.cc
    src/clients/cute_client.cc
    src/clients/influx_client.cc
    src/clients/client.cc
    src/clients/client_factory.cc
    src/main.cc
    src/proto/packet.pb.cc
    src/protocol_parser.cc
    src/protocol.cc
    src/log.cc
    src/app.cc
    src/util.cc
)

SET( HEADERS
    inc/producers/fake_producer.h
    inc/producers/serial_producer.h
    inc/producers/producer.h
    inc/producers/producer_factory.h
    inc/clients/cute_client.h
    inc/clients/influx_client.h
    inc/clients/client.h
    inc/clients/client_factory.h
    inc/proto/packet.h
    inc/proto/packet.pb.h
    inc/protocol_parser.h
    inc/protocol.h
    inc/log.h
    inc/app.h
    inc/util.h
    shared/interfaces/radio/radio_packet.h
    shared/interfaces/id.h
)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

# Qt5
set (CMAKE_PREFIX_PATH /opt/Qt/5.12.1/gcc_64/)
find_package(Qt5 COMPONENTS Core Network Xml SerialPort REQUIRED)

# Protobuf, for messages
find_package(Protobuf REQUIRED)

add_executable(rocketsd ${SOURCES} ${HEADERS} ${UI})
set_property(TARGET rocketsd PROPERTY CXX_STANDARD 17)
target_link_libraries(rocketsd Qt5::Network Qt5::Xml Qt5::SerialPort protobuf::libprotobuf gcov)
target_include_directories(rocketsd PRIVATE . inc)
target_compile_options(rocketsd PRIVATE -Wall -Wpedantic)
