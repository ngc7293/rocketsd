cmake_minimum_required(VERSION 3.1.0)
project(rocketsd)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set( SOURCES
    src/app.cc
    src/main.cc
)

SET( HEADERS
    inc/app.h
)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

# Qt5
set (CMAKE_PREFIX_PATH /opt/Qt/5.12.1/gcc_64/)
find_package(Qt5 COMPONENTS Core Network REQUIRED)

# Protobuf, for messages
find_package(Protobuf REQUIRED)

add_executable(rocketsd ${SOURCES} ${HEADERS} ${UI})
set_property(TARGET rocketsd PROPERTY CXX_STANDARD 17)
target_link_libraries(rocketsd Qt5::Network protobuf::libprotobuf gcov)
target_include_directories(rocketsd PRIVATE inc)
target_compile_options(rocketsd PRIVATE -Wall -Wpedantic)
