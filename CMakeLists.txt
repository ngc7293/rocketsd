cmake_minimum_required(VERSION 3.1.0)
project(rocketsd)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set( SOURCES
    src/main.cc
    src/protocol.cc
    src/protocol_parser.cc
)

SET( HEADERS
    inc/protocol.h
    inc/protocol_parser.h
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
target_include_directories(rocketsd PRIVATE inc)
target_compile_options(rocketsd PRIVATE -Wall -Wpedantic)
