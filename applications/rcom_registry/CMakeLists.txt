cmake_minimum_required(VERSION 3.10)

include_directories(include)

add_executable(rcom-registry
                src/rcom-registry.cpp)

target_link_libraries(rcom-registry rcom rpp r m)


#if(BUILD_TESTS)
#    add_subdirectory(test)
#endif()