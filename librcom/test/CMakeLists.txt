cmake_minimum_required(VERSION 3.10)

set(SRCS
        src/tests_main.cpp
        src/address_tests.cpp
        src/util_tests.cpp
        src/rawmessagehub_tests.cpp
        src/registry_tests.cpp
        src/response_tests.cpp
        src/request_tests.cpp
        src/requestparser_tests.cpp
        src/responseparser_tests.cpp
        src/serverside_websocket_tests.cpp
        mocks/ServerSocket.mock.h
        mocks/SocketFactory.mock.h
        mocks/MessageListener.mock.h
        src/websocketserver_tests.cpp
        src/serversocket_tests.cpp
        src/serverside_websocket_tests.cpp
        src/clientside_websocket_tests.cpp
        )

add_executable(librcom_unit_tests ${SRCS})

target_link_libraries(librcom_unit_tests
                        gtest
                        gmock
                        rcommocks
                        rcom)
 
#target_include_directories(librcom_unit_tests
#        PRIVATE
#        ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_test(
        NAME librcom_unit_tests
        COMMAND librcom_unit_tests
        )

if (BUILD_COVERAGE)
        SETUP_TARGET_FOR_COVERAGE_LCOV(
                            NAME librcom_unit_tests_coverage
                            EXECUTABLE ctest -V ${n_cores} -R "librcom"
                            EXCLUDE "src/rcom-registry.cpp" "include/*" "examples/*"
                            DEPENDENCIES librcom_unit_tests)
endif()
