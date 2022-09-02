cmake_minimum_required(VERSION 3.10)

include_directories(include)

add_executable(rcom-registry
                src/rcom-registry.cpp)

<<<<<<< HEAD
target_link_libraries(rcom-registry rcom m)
=======
target_link_libraries(rcom-registry rcom rpp m)
>>>>>>> 05dd7cfbc6a114654c64e9d7e489aab6ad3ab18b


#if(BUILD_TESTS)
#    add_subdirectory(test)
#endif()
