cmake_minimum_required(VERSION 3.17.0)
project(rocketsd VERSION 1.0.1)

# Third parties
include("${CMAKE_BINARY_DIR}/conan_paths.cmake")
include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup(TARGETS)

# Qt5
include(conan_qt_executables_variables)
cmake_policy(SET CMP0100 NEW)

if (UNIX)
    target_compile_options(CONAN_PKG::qt INTERFACE -fPIC)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# GTest
find_package(GTest)
find_package(Protobuf)
enable_testing()

# Installation stuff
if (UNIX)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "David Bourgault")
    set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/rocketsd")
elseif(MSVC)
    set(CPACK_GENERATOR "WIX")
endif()

include(CPack)
include(InstallRequiredSystemLibraries)

function(install_lib_with_rpath)
    cmake_parse_arguments(
        INSTALL_LIB
        ""
        "DESTINATION;RPATH"
        "FILES"
        ${ARGN}
    )

    if(UNIX)
        foreach(FILE ${INSTALL_LIB_FILES})
            if(NOT IS_SYMLINK ${FILE})
                install(CODE "execute_process(COMMAND chrpath -r ${INSTALL_LIB_RPATH} ${FILE})")
            endif()
        endforeach()
    endif()

    install(FILES ${INSTALL_LIB_FILES} DESTINATION ${INSTALL_LIB_DESTINATION})
endfunction()

function(install_qt)
    cmake_parse_arguments(
        INSTALL_QT
        "PLATFORM"
        ""
        "LIBS;LIBS_LINUX;LIBS_WINDOWS"
        ${ARGN}
    )

    if(UNIX)
        foreach(QTLIB ${INSTALL_QT_LIBS} ${INSTALL_QT_LIBS_LINUX})
            install_lib_with_rpath(FILES ${CMAKE_BINARY_DIR}/lib/libQt6${QTLIB}.so.6.2.4 ${CMAKE_BINARY_DIR}/lib/libQt6${QTLIB}.so.6 DESTINATION lib RPATH "\$ORIGIN")
        endforeach()
    
        if(INSTALL_QT_PLATFORM)
            install_lib_with_rpath(FILES ${CMAKE_BINARY_DIR}/res/archdatadir/plugins/platforms/libqxcb.so DESTINATION bin/platforms RPATH "\$ORIGIN/../../lib")
            install_lib_with_rpath(FILES ${CMAKE_BINARY_DIR}/res/archdatadir/plugins/xcbglintegrations/libqxcb-glx-integration.so DESTINATION bin/xcbglintegrations RPATH "\$ORIGIN/../../lib")
        endif()
    endif()

    if(MSVC)
        foreach(QTLIB ${INSTALL_QT_LIBS} ${INSTALL_QT_LIBS_WINDOWS})
            install(FILES ${CMAKE_BINARY_DIR}/bin/Qt6${QTLIB}.dll DESTINATION bin)
        endforeach()

        if(INSTALL_QT_PLATFORM)
            install(FILES ${CMAKE_BINARY_DIR}/res/archdatadir/plugins/platforms/qwindows.dll DESTINATION bin/platforms)
            install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION bin)
        endif()
    endif()
endfunction()

# Functions
function(target_os_compile_options)
    cmake_parse_arguments(
        OS_COMPILE_OPTIONS
        ""
        "TARGET"
        "LINUX;WINDOWS;ALL"
        ${ARGN}
    )

    if (UNIX)
        target_compile_options(${OS_COMPILE_OPTIONS_TARGET} PRIVATE ${OS_COMPILE_OPTIONS_LINUX})
    endif()

    if (MSVC)
        target_compile_options(${OS_COMPILE_OPTIONS_TARGET} PRIVATE ${OS_COMPILE_OPTIONS_WINDOWS})
    endif()

    target_compile_options(${OS_COMPILE_OPTIONS_TARGET} PRIVATE ${OS_COMPILE_OPTIONS_ALL})
endfunction()

function(target_standard_compile_flags TARGET)
    target_os_compile_options(
        TARGET ${TARGET}
        LINUX -Wall -Werror -Wpedantic -Wno-unknown-pragmas
        WINDOWS /W4 /WX /wd4068 /wd4127
    )

    if (CMAKE_BUILD_TYPE STREQUAL Debug)
        if (UNIX)
            target_compile_options(${TARGET} PRIVATE --coverage)
            target_link_options(${TARGET} PRIVATE --coverage)
            target_link_libraries(${TARGET} PUBLIC gcov)
        endif()
    endif()

    target_compile_features(${TARGET} PUBLIC cxx_std_20)
endfunction()

if (UNIX)
    add_custom_target(ids
        COMMAND xmlprotoc/xmlprotoc.py --prepend xmlprotoc/id_head.h.pc --append xmlprotoc/id_tail.h.pc --out xmlprotoc/id.h protocol.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )

    add_custom_target(clean_gcda ALL COMMAND find -name "*.gcda" -delete COMMENT "Cleaning GCDA files")
    add_custom_target(coverage COMMAND gcovr -r ../src ./ -e ".*gtest.*" -e ".*/tests/.*" --html-details -o coverage.html)
endif()

add_subdirectory(libs)
add_subdirectory(src)
