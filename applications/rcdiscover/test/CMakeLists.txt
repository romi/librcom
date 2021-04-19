cmake_minimum_required(VERSION 3.10)

include_directories(mocks
        support)

set(SRCS
        support/TestUtil.hpp
        src/tests_main.cpp
        src/SerialPortIdentification_tests.cpp
        src/SerialPortDiscover_tests.cpp
        src/SerialPortConfigurationGenerator_tests.cpp
        ../include/ISerialPortConfigurationGenerator.h
        ../include/SerialPortConfigurationGenerator.h
        ../src/SerialPortConfigurationGenerator.cpp
        ../include/ISerialPortIdentification.h
        ../include/SerialPortIdentification.h
        ../src/SerialPortIdentification.cpp
        ../include/ISerialPortDiscover.h
        ../include/SerialPortDiscover.h
        ../src/SerialPortDiscover.cpp)

add_executable(rcom_rcdiscover_unit_tests ${SRCS})

target_link_libraries(  rcom_rcdiscover_unit_tests
                        gtest
                        gmock
                        rpp
                        r)

add_test(
        NAME rcom_rcdiscover_unit_tests
        COMMAND rcom_rcdiscover_unit_tests
        )
