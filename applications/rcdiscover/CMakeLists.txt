cmake_minimum_required(VERSION 3.10)

include_directories(include
        src)

add_executable(rcdiscover
                src/main.cpp
                include/SerialPortConfigurationGenerator.h
                src/SerialPortConfigurationGenerator.cpp
                include/SerialPortIdentification.h
                src/SerialPortIdentification.cpp
                include/SerialPortDiscover.h
                src/SerialPortDiscover.cpp
        )

target_link_libraries( rcdiscover
                        stdc++fs
                        rpp)

if(BUILD_TESTS)
    add_subdirectory(test)
endif()