
cmake_minimum_required(VERSION 3.10)
project(librcom-doc)

# monster_1
add_executable(monster_1 monster_1.cpp)
INSTALL(TARGETS monster_1 DESTINATION "bin")

# monster_2
add_executable(monster_2 monster_2.cpp)
target_link_libraries(monster_2 rcom)
INSTALL(TARGETS monster_2 DESTINATION "bin")

# monster_3
add_executable(monster_3 monster_3.cpp)
target_link_libraries(monster_3 rcom)
INSTALL(TARGETS monster_3 DESTINATION "bin")
