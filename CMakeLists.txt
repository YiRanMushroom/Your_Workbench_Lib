cmake_minimum_required(VERSION 3.31)

set(PROJECT_NAME your_workbench_lib)

project(your_workbench_lib
        VERSION 0.1.0
        DESCRIPTION "modular C++ utility library"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE MODULES "ywl/*.cppm" "ywl/*.ixx")

add_library(${PROJECT_NAME} ${MODULES}
        ywl/misc/coroutine/coroutine.ixx
        ywl/basic/copiable_reference.cppm)

target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES ${MODULES}
)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        FILE_SET cxx_modules
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

if(WIN32)
    target_link_libraries(your_workbench_lib PRIVATE DbgHelp)
endif()

target_precompile_headers(your_workbench_lib
        PUBLIC
        ywl/pch/stdc++pch.hpp
)

install(FILES ywl/pch/stdc++pch.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ywl/pch
)

target_include_directories(your_workbench_lib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ywl/pch>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/ywl/pch>
)