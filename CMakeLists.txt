
cmake_minimum_required(VERSION 3.10)

include(CMakePackageConfigHelpers)
include(cmake/version.cmake)

if(DEFINED ENV{CICD_NAME})
    project ("$ENV{CICD_NAME}" LANGUAGES C CXX)
else()
    project (sqlite_bench LANGUAGES C CXX)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
########################
# Version

add_custom_target(version
        COMMAND ${CMAKE_COMMAND} -P cmake/version.cmake
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

###########################
# Dependencies

set(CMAKE_PREFIX_PATH $ENV{OECORE_TARGET_SYSROOT}/usr/lib/cmake)
message("CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

find_package(Threads REQUIRED)

find_package(Boost 1.54.0 REQUIRED COMPONENTS)
add_compile_definitions(BOOST_BIND_GLOBAL_PLACEHOLDERS)

find_package(SQLite3 REQUIRED)

find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
#######################
# Generate library

set(LIBRARY_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/databank.cpp
)

set(LIBRARY_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)


add_library(${PROJECT_NAME} SHARED
    ${LIBRARY_SOURCES}
    ${CMAKE_BINARY_DIR}/version.h
)


set_target_properties(${PROJECT_NAME}
    PROPERTIES PUBLIC_HEADER
    "include/databank.hpp"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${LIBRARY_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${SQLite3_INCLUDE_DIRS}        
    ${CMAKE_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${Boost_LIBRARIES}
    ${SQLite3_LIBRARIES}
    Threads::Threads
    spdlog::spdlog_header_only
)

target_compile_definitions(${PROJECT_NAME} PUBLIC FMT_HEADER_ONLY=1)


set(APP_TARGET ${PROJECT_NAME}_EXE)

set(BENCHMARK_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)


add_executable(${APP_TARGET}
        ${LIBRARY_SOURCES}
        main.cpp
        src/benchmarkSenOne.cpp
        src/benchmarkSenTwo.cpp
)

target_include_directories(${APP_TARGET} PRIVATE
        ${LIBRARY_INCLUDE_DIRS}
        ${BENCHMARK_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_BINARY_DIR}
)

target_link_libraries(${APP_TARGET}
        ${Boost_LIBRARIES}
        ${SQLite3_LIBRARIES}
        spdlog::spdlog_header_only
)