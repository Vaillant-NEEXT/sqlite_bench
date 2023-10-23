
cmake_minimum_required(VERSION 3.10)

include(CMakePackageConfigHelpers)
include(cmake/version.cmake)

if(DEFINED ENV{CICD_NAME})
    project ("$ENV{CICD_NAME}" LANGUAGES C CXX)
else()
    project (bsw-recorder LANGUAGES C CXX)
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

find_package(Boost 1.54.0 REQUIRED COMPONENTS program_options)
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

#######################
# Enable testing on QEMU

set(CMAKE_CROSSCOMPILING_EMULATOR "$ENV{SDK_QEMU}")

#######################
# Testing

if(BUILD_TESTING)
    add_subdirectory(test/unit-test)
endif(BUILD_TESTING)

if(APP)
    add_subdirectory(app)
endif(APP)

#######################
# Write version
#[[
set_target_properties(${PROJECT_NAME}
        PROPERTIES
        VERSION ${GIT_TAG_VERISON}
        SOVERSION ${GIT_VERSION_MAJOR}
        INTERFACE_${PROJECT_NAME}_MAJOR_VERSION ${GIT_VERSION_MAJOR}
)

set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING ${PROJECT_NAME}_MAJOR_VERSION
)
]]
#######################
# Generate the version file for the config file
#[[
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/neext-${PROJECT_NAME}-config-version.cmake"
    VERSION "${GIT_TAG_VERISON}"
    COMPATIBILITY AnyNewerVersion
)        
]]
#######################
# Install target for lib
#[[
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/neext/bsw/persistency
)

# install export target and config for find_packacge
install(EXPORT ${PROJECT_NAME}-targets
    FILE neext-${PROJECT_NAME}-targets.cmake
    DESTINATION lib/cmake/neext-${PROJECT_NAME}
)
]]
#######################
# Create package config

#[[
configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in 
    "${CMAKE_CURRENT_BINARY_DIR}/neext-${PROJECT_NAME}-config.cmake"
	INSTALL_DESTINATION lib/cmake/neext-${PROJECT_NAME}
)
    
install(FILES 
    "${CMAKE_CURRENT_BINARY_DIR}/neext-${PROJECT_NAME}-config.cmake" 
    "${CMAKE_CURRENT_BINARY_DIR}/neext-${PROJECT_NAME}-config-version.cmake" 
    DESTINATION lib/cmake/neext-${PROJECT_NAME}
)
]]