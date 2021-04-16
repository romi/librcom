/*
  rcom

  Copyright (C) 2019 Sony Computer Science Laboratories
  Author(s) Peter Hanappe

  rcom is light-weight libary for inter-node communication.

  rcom is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see
  <http://www.gnu.org/licenses/>.

 */
#ifndef _RCOM_WEBSOCKET_CONSTANTS_H_
#define _RCOM_WEBSOCKET_CONSTANTS_H_

namespace rcom {
                                
        enum Opcode {
                kContinutationOpcode = 0,
                kTextOpcode = 1,
                kBinaryOpcode = 2,
                kCloseOpcode = 8,
                kPingOpcode = 9,
                kPongOpcode = 10
        };

        enum CloseCode {
        
                /* 1000 indicates a normal closure, meaning that the
                   purpose for which the connection was established
                   has been fulfilled. */
                kCloseNormal = 1000,
        
                /* 1001 indicates that an endpoint is "going away",
                   such as a server going down or a browser having
                   navigated away from a page. */
                kCloseGoingAway = 1001,

                /* 1002 indicates that an endpoint is terminating the
                 * connection due to a protocol error. */
                kCloseProtocolError = 1002,

                /* 1003 indicates that an endpoint is terminating the
                 * connection because it has received a type of data
                 * it cannot accept (e.g., an endpoint that
                 * understands only text data MAY send this if it
                 * receives a binary message). */
                kCloseUnhandledData = 1003,

                /* 1005 is a reserved value and MUST NOT be set as a
                 * status code in a Close control frame by an
                 * endpoint. It is designated for use in applications
                 * expecting a status code to indicate that no status
                 * code was actually present. */
                kCloseMissingCode = 1005,

                /* 1006 is a reserved value and MUST NOT be set as a
                 * status code in a Close control frame by an
                 * endpoint. It is designated for use in applications
                 * expecting a status code to indicate that the
                 * connection was closed abnormally, e.g., without
                 * sending or receiving a Close control frame. */
                kCloseUnexpected = 1006,

                /* 1007 indicates that an endpoint is terminating the
                 * connection because it has received data within a
                 * message that was not consistent with the type of
                 * the message (e.g., non-UTF-8 [RFC3629] data within
                 * a text message). */
                kCloseInvalidData = 1007,

                /* 1008 indicates that an endpoint is terminating the
                 * connection because it has received a message that
                 * violates its policy.  This is a generic status code
                 * that can be returned when there is no other more
                 * suitable status code (e.g., 1003 or 1009) or if
                 * there is a need to hide specific details about the
                 * policy. */
                kClosePolicyViolation = 1008,

                /* 1009 indicates that an endpoint is terminating the
                 * connection because it has received a message that
                 * is too big for it to process. */
                kCloseTooBig = 1009,

                /* 1010 indicates that an endpoint (client) is
                 * terminating the connection because it has expected
                 * the server to negotiate one or more extension, but
                 * the server didn't return them in the response
                 * message of the WebSocket handshake. */
                kCloseMissingExtensions = 1010,
                
                /* 1011 indicates that a server is terminating the
                 * connection because it encountered an unexpected
                 * condition that prevented it from fulfilling the
                 * request. */
                kCloseInternalError = 1011
                
        };
}

#endif // _RCOM_WEBSOCKET_CONSTANTS_H_
