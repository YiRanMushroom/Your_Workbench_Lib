cmake_minimum_required(VERSION 3.31)

set(PROJECT_NAME your_workbench_lib)

project(your_workbench_lib
        VERSION 0.1.0
        DESCRIPTION "modular C++ utility library"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# use libc++ instead of libstdc++ on all platforms
if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -stdlib=libc++")
else ()
    # cmake error
    message(FATAL_ERROR "MSVC is not supported")
endif()

file(GLOB_RECURSE MODULES "ywl/*.cppm" "ywl/*.ixx")

add_library(${PROJECT_NAME} ${MODULES}
        ywl/miscellaneous/multithreading/multithreading.ixx
        ywl/miscellaneous/multithreading/channel.cppm
        ywl/miscellaneous/rust.cppm
        ywl/util/enum_entry.cppm
        ywl/miscellaneous/multithreading/thread_pool.cppm)

target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES ${MODULES}
)

#[[if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_target_properties(${PROJECT_NAME} PROPERTIES
            MSVC_RUNTIME_LIBRARY ${CMAKE_MSVC_RUNTIME_LIBRARY})
endif()]]

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