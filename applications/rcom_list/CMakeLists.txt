cmake_minimum_required(VERSION 3.10)

include_directories(include)

add_executable(rcom-list rcom-list.cpp)
target_link_libraries(rcom-list rcom m)
