
project(rcommocks)

set(SOURCES
  Address.mock.h
  Clock.mock.h
  MessageListener.mock.h
  Request.mock.h
  RequestParser.mock.h
  Response.mock.h
  ResponseParser.mock.h
  ServerSocket.mock.h
  SocketFactory.mock.h
  Socket.mock.h
  WebSocket.mock.h
  WebSocketServer.mock.h
  WebSocketServerFactory.mock.h)

add_library(rcommocks SHARED ${SOURCES})
set_target_properties(rcommocks PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(rcommocks
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )
