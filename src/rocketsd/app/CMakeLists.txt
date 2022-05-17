cmake_minimum_required(VERSION 3.17)

set(SOURCES
    app.cc
    app.hh
    main.cc
)

add_executable(rocketsd ${SOURCES} ${HEADERS})
target_standard_compile_flags(rocketsd)
target_link_libraries(rocketsd PUBLIC rocketsd.module cute.proto CONAN_PKG::qt)

set_target_properties(rocketsd PROPERTIES INSTALL_RPATH "\$ORIGIN/../lib")
install(TARGETS rocketsd DESTINATION bin)
install_qt(LIBS Core Network SerialPort Xml)