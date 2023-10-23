
find_package(GTest REQUIRED)

enable_testing()

set(TEST_TARGET ${PROJECT_NAME}-unit-test)

add_executable(${TEST_TARGET}
        ${LIBRARY_SOURCES}
        main.cpp
        test_api.cpp
        test_config.cpp
        test_query.cpp
)

target_include_directories(${TEST_TARGET} PRIVATE
        ${LIBRARY_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${GTEST_INCLUDE_DIRS}
        ${CMAKE_BINARY_DIR}

)

target_link_libraries(${TEST_TARGET}
        gtest
        gtest_main
        ${Boost_LIBRARIES}
        ${SQLite3_LIBRARIES}
        spdlog
        fmt::fmt-header-only
)

add_compile_definitions(TEST_INI_DIR="${CMAKE_CURRENT_SOURCE_DIR}/conf")

gtest_discover_tests(${TEST_TARGET} XML_OUTPUT_DIR ./${TEST_TARGET}.xml)
add_test(NAME unittests COMMAND ${TEST_TARGET} --gtest_output="xml:./${TEST_TARGET}.xml")
