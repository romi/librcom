
cmake_minimum_required(VERSION 3.10)
project(librcompp-examples)

# hello-world
add_executable(rpc-hello-world-server rpc-hello-world/server.cpp)
target_link_libraries(rpc-hello-world-server rcompp-lite rpp r m)
INSTALL(TARGETS rpc-hello-world-server DESTINATION "bin")

add_executable(rpc-hello-world-client rpc-hello-world/client.cpp)
target_link_libraries(rpc-hello-world-client rcompp-lite rpp r m)
INSTALL(TARGETS rpc-hello-world-client DESTINATION "bin")


# sensor
add_executable(pubsub-sensor-publisher pubsub-sensor/publisher.cpp)
target_link_libraries(pubsub-sensor-publisher rcompp-lite rpp r m)
INSTALL(TARGETS pubsub-sensor-publisher DESTINATION "bin")

add_executable(sensor-subscriber pubsub-sensor/subscriber.cpp)
target_link_libraries(sensor-subscriber rcompp-lite rpp r m)
INSTALL(TARGETS sensor-subscriber DESTINATION "bin")


# chat
add_executable(messagebus-chat-server messagebus/chat-server.cpp)
target_link_libraries(messagebus-chat-server rcompp-lite rpp r m)
INSTALL(TARGETS messagebus-chat-server DESTINATION "bin")

add_executable(messagebus-chat-client messagebus/chat-client.cpp)
target_link_libraries(messagebus-chat-client rcompp-lite rpp r m)
INSTALL(TARGETS messagebus-chat-client DESTINATION "bin")


# speed
add_executable(rpc-speed-test-server rpc-speed-test/server.cpp)
target_link_libraries(rpc-speed-test-server rcompp-lite rpp r m)
INSTALL(TARGETS rpc-speed-test-server DESTINATION "bin")

add_executable(rpc-speed-test-client rpc-speed-test/client.cpp)
target_link_libraries(rpc-speed-test-client rcompp-lite rpp r m)
INSTALL(TARGETS rpc-speed-test-client DESTINATION "bin")


