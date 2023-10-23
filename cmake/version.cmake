
execute_process(COMMAND git describe --tags --abbrev --match "v[0-9].[0-9].[0-9]" --long
        OUTPUT_VARIABLE GIT_DESCRIBE
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        ERROR_QUIET) 

set(GIT_DEFAULT_VERSION "0")
set(VERSION_HEADER "v")
set(VERSION ${GIT_DEFAULT_VERSION})

if("${GIT_DESCRIBE}" STREQUAL "")
        # no  git tag use default value "v0.0.0-0"
        set(GIT_VERSION_MAJOR ${GIT_DEFAULT_VERSION})
        set(GIT_VERSION_MINOR ${GIT_DEFAULT_VERSION})
        set(GIT_VERSION_PATCH ${GIT_DEFAULT_VERSION})
        set(GIT_VERSION_SHA1 ${GIT_DEFAULT_VERSION})

        set(VERSION "${VERSION_HEADER}${GIT_VERSION_MAJOR}.${GIT_VERSION_MINOR}.${GIT_VERSION_PATCH}-${GIT_VERSION_SHA1}")
        message("Default Version: ${VERSION}")
else()
        # strip any trailing spaces
        string(STRIP ${GIT_DESCRIBE} VERSION)
        message("Git tag Version: ${VERSION}")

        #parse the version information
        string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" GIT_VERSION_MAJOR "${VERSION}")
        string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" GIT_VERSION_MINOR "${VERSION}")
        string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" GIT_VERSION_PATCH "${VERSION}")
        string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" GIT_VERSION_SHA1 "${VERSION}")
endif()

set(GIT_TAG_VERISON "${GIT_VERSION_MAJOR}.${GIT_VERSION_MINOR}.${GIT_VERSION_PATCH}")
message("GIT_VERSION_MAJOR: ${GIT_VERSION_MAJOR}")
message("GIT_VERSION_MINOR: ${GIT_VERSION_MINOR}")
message("GIT_VERSION_PATCH: ${GIT_VERSION_PATCH}")

configure_file(${CMAKE_SOURCE_DIR}/src/version.h.in ${CMAKE_BINARY_DIR}/version.h)
message("${CMAKE_BINARY_DIR}/version.h generated")
