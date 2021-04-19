
cmake_minimum_required(VERSION 3.10)
project(librcom)

include(${CMAKE_MODULES_DIRECTORY}/RomiProjectBuildOptions.cmake)

set(SOURCES
    include/sha1.h
    include/util.h
    include/Address.h
    include/BaseSocket.h
    include/ClientSideWebSocket.h
    include/Clock.h
    include/HttpParser.h
    include/IAddress.h
    include/IClock.h
    include/IHttpParserListener.h
    include/IMessageHub.h
    include/IMessageLink.h
    include/IMessageListener.h
    include/IRequest.h
    include/IRequestParser.h
    include/IResponse.h
    include/IServerSocket.h
    include/ISocket.h
    include/ISocketFactory.h
    include/IWebSocketServer.h
    include/MessageLink.h
    include/MessageHub.h
    include/RegistryEntry.h
    include/Registry.h
    include/RegistryProxy.h
    include/RegistryServer.h
    include/RequestParser.h
    include/Response.h
    include/ResponseParser.h
    include/Socket.h
    include/SocketFactory.h
    include/ServerSideWebSocket.h
    include/ServerSocket.h
    include/WebSocket.h
    include/WebSocketConstants.h
    include/WebSocketServer.h
    
    src/sha1.cpp
    src/util.cpp
    src/Address.cpp
    src/BaseSocket.cpp
    src/ClientSideWebSocket.cpp
    src/Clock.cpp
    src/HttpParser.cpp
    src/MessageLink.cpp
    src/MessageHub.cpp
    src/RegistryEntry.cpp
    src/Registry.cpp
    src/RegistryProxy.cpp
    src/RegistryServer.cpp
    src/Request.cpp
    src/RequestParser.cpp
    src/Response.cpp
    src/ResponseParser.cpp
    src/ServerSideWebSocket.cpp
    src/ServerSocket.cpp
    src/Socket.cpp
    src/SocketFactory.cpp
    src/WebSocket.cpp
    src/WebSocketServer.cpp

    include/IRPCClient.h
    include/IRPCHandler.h
    include/RPCClient.h
    include/RPCError.h
    include/RPCServer.h
    src/RPCClient.cpp
    src/RPCServer.cpp
)

add_library(rcompp-lite SHARED ${SOURCES})
target_include_directories(rcompp-lite PUBLIC include)
target_link_libraries(rcompp-lite m rpp r)

#
#
#add_executable(rcom-registry ../applications/rcom_registry/src/rcom-registry.cpp)
#target_link_libraries(rcom-registry rcompp-lite rpp r m)
##INSTALL(TARGETS rcom-registry DESTINATION "bin")
      
if(BUILD_TESTS)
    add_subdirectory(test)
endif()

add_subdirectory(examples)

