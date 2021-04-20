
project(rcom-mocks)

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
  WebSocket.mock.h)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../include")
add_library(rcom-mocks SHARED ${SOURCES})
set_target_properties(rcom-mocks PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(rcom-mocks PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
