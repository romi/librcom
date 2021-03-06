
cmake_minimum_required(VERSION 3.10)
project(librcom)

############################################################

output_compile_flags()

############################################################

set(SOURCES
    include/sha1.h
    include/util.h
    include/ip.h
    include/Address.h
    include/BaseSocket.h
    include/ClientSideWebSocket.h
    include/ConsoleLogger.h
    include/DummyMessageListener.h
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
    include/IWebSocketServerFactory.h
    include/json.hpp
    include/MemBuffer.h
    include/MessageLink.h
    include/MessageHub.h
    include/RawMessageHub.h
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
    include/WebSocketServerFactory.h
    
    src/sha1.cpp
    src/util.cpp
    src/ip.cpp
    src/Address.cpp
    src/BaseSocket.cpp
    src/ClientSideWebSocket.cpp
    src/ConsoleLogger.cpp
    src/HttpParser.cpp
    src/MemBuffer.cpp
    src/MessageLink.cpp
    src/MessageHub.cpp
    src/RawMessageHub.cpp
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
    src/WebSocketServerFactory.cpp
        include/Frames.h)

add_library(rcom SHARED ${SOURCES})
target_include_directories(rcom PUBLIC include)
target_link_libraries(rcom m rpp)

add_subdirectory(test/mocks)

if(BUILD_TESTS)
    add_subdirectory(test)
endif()

add_subdirectory(examples)

