
cmake_minimum_required(VERSION 3.10)
project(librcom)

############################################################

output_compile_flags()

############################################################

set(SOURCES
    include/rcom/sha1.h
    include/rcom/util.h
    include/rcom/ip.h
    include/rcom/Address.h
    include/rcom/BaseSocket.h
    include/rcom/ClientSideWebSocket.h
    include/rcom/ConsoleLog.h
    include/rcom/DummyMessageListener.h
    include/rcom/Frames.h
    include/rcom/HttpParser.h
    include/rcom/IAddress.h
    include/rcom/IHttpParserListener.h
    include/rcom/ILinux.h
    include/rcom/ILog.h
    include/rcom/IMessageHub.h
    include/rcom/IMessageLink.h
    include/rcom/IMessageListener.h
    include/rcom/IRequest.h
    include/rcom/IRequestParser.h
    include/rcom/IResponse.h
    include/rcom/IRPCHandler.h
    include/rcom/IRPCServer.h
    include/rcom/IServerSocket.h
    include/rcom/ISocket.h
    include/rcom/ISocketFactory.h
    include/rcom/IWebSocketServer.h
    include/rcom/json.hpp
    include/rcom/Linux.h
    include/rcom/Log.h
    include/rcom/MemBuffer.h
    include/rcom/MessageLink.h
    include/rcom/MessageHub.h
    include/rcom/RcomClient.h
    include/rcom/RcomMessageHandler.h
    include/rcom/RcomServer.h
    include/rcom/RegistryEntry.h
    include/rcom/Registry.h
    include/rcom/RegistryLookup.h
    include/rcom/RegistryLookupServer.h
    include/rcom/RegistryProxy.h
    include/rcom/RegistryServer.h
    include/rcom/RemoteStub.h
    include/rcom/RemoteObjectsAdaptor.h
    include/rcom/RequestParser.h
    include/rcom/Response.h
    include/rcom/ResponseParser.h
    include/rcom/Socket.h
    include/rcom/SocketFactory.h
    include/rcom/ServerSideWebSocket.h
    include/rcom/ServerSocket.h
    include/rcom/WebSocket.h
    include/rcom/WebSocketConstants.h
    include/rcom/WebSocketServer.h
    
    src/sha1.cpp
    src/util.cpp
    src/ip.cpp
    src/Address.cpp
    src/BaseSocket.cpp
    src/ClientSideWebSocket.cpp
    src/ConsoleLog.cpp
    src/HttpParser.cpp
    src/Linux.cpp
    src/Log.cpp
    src/MemBuffer.cpp
    src/MessageLink.cpp
    src/MessageHub.cpp
    src/RcomClient.cpp
    src/RcomMessageHandler.cpp
    src/RcomServer.cpp
    src/RegistryEntry.cpp
    src/Registry.cpp
    src/RegistryLookup.cpp
    src/RegistryLookupServer.cpp
    src/RegistryProxy.cpp
    src/RegistryServer.cpp
    src/Request.cpp
    src/RemoteStub.cpp
    src/RemoteObjectsAdaptor.cpp
    src/RequestParser.cpp
    src/Response.cpp
    src/ResponseParser.cpp
    src/ServerSideWebSocket.cpp
    src/ServerSocket.cpp
    src/Socket.cpp
    src/SocketFactory.cpp
    src/WebSocket.cpp
    src/WebSocketServer.cpp)

add_library(rcom SHARED ${SOURCES})
target_include_directories(rcom PUBLIC include)
target_link_libraries(rcom i2c m)

add_subdirectory(test/mocks)

if(BUILD_TESTS)
    add_subdirectory(test)
endif()

