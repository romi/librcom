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
#ifndef _LIBRCOM_RPCERROR_H
#define _LIBRCOM_RPCERROR_H

#include <string>

namespace rcom {
                                                      
        class RPCError
        {
        public:
                enum {
                        kParseError = -32700,     // Invalid JSON received by the server.
                        kInvalidRequest = -32600, // The JSON request is not a valid.
                        kMethodNotFound = -32601, // The handler doesn't know the method.
                        kInvalidParams = -32602,  // Invalid parameter(s).
                        kSendError = -32603,      // Sending failed.
                        kReceiveError = -32604,   // The client failed to read the response.
                        kReceiveTimeout = -32605, // The response timed out.
                        kLinkClosed = -32606,     // The link was closed.
                        kInternalError = -32607,  // Internal error.
                        
                        kNullMethod = -32000,      // The method was null.
                        kInvalidResponse = -32001, // The JSON response is not a valid.
                };
                        
                int16_t code;
                std::string message;
        public:
                RPCError() : code(0), message() {};
        };
}

#endif // _LIBRCOM_RPCERROR_H
