
cmake_minimum_required(VERSION 3.10)
project(librcom)

set(SOURCES
    include/sha1.h
    include/util.h
    include/ip.h
    include/Address.h
    include/BaseSocket.h
    include/ClientSideWebSocket.h
    include/HttpParser.h
    include/IAddress.h
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
    src/ip.cpp
    src/Address.cpp
    src/BaseSocket.cpp
    src/ClientSideWebSocket.cpp
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
)

add_library(rcompp-lite SHARED ${SOURCES})
target_include_directories(rcompp-lite PUBLIC include)
target_link_libraries(rcompp-lite m rpp)

add_subdirectory(test/mocks)

if(BUILD_TESTS)
    add_subdirectory(test)
endif()

add_subdirectory(examples)

