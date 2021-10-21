
cmake_minimum_required(VERSION 3.10)
project(librcom-examples)

# hello-world
add_executable(rpc-hello-world-server rpc-hello-world/server.cpp)
target_link_libraries(rpc-hello-world-server rcom rpp r m)
INSTALL(TARGETS rpc-hello-world-server DESTINATION "bin")

add_executable(rpc-hello-world-client rpc-hello-world/client.cpp)
target_link_libraries(rpc-hello-world-client rcom rpp r m)
INSTALL(TARGETS rpc-hello-world-client DESTINATION "bin")


# sensor
add_executable(pubsub-sensor-publisher pubsub-sensor/publisher.cpp)
target_link_libraries(pubsub-sensor-publisher rcom rpp r m)
INSTALL(TARGETS pubsub-sensor-publisher DESTINATION "bin")

add_executable(sensor-subscriber pubsub-sensor/subscriber.cpp)
target_link_libraries(sensor-subscriber rcom rpp r m)
INSTALL(TARGETS sensor-subscriber DESTINATION "bin")


# chat
add_executable(messagebus-chat-server messagebus/chat-server.cpp)
target_link_libraries(messagebus-chat-server rcom rpp r m)
INSTALL(TARGETS messagebus-chat-server DESTINATION "bin")

add_executable(messagebus-chat-client messagebus/chat-client.cpp)
target_link_libraries(messagebus-chat-client rcom rpp r m)
INSTALL(TARGETS messagebus-chat-client DESTINATION "bin")


# speed
add_executable(rpc-speed-test-server rpc-speed-test/server.cpp)
target_link_libraries(rpc-speed-test-server rcom rpp r m)
INSTALL(TARGETS rpc-speed-test-server DESTINATION "bin")

add_executable(rpc-speed-test-client rpc-speed-test/client.cpp)
target_link_libraries(rpc-speed-test-client rcom rpp r m)
INSTALL(TARGETS rpc-speed-test-client DESTINATION "bin")


