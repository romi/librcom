/*
  romi-rover

  Copyright (C) 2019 Sony Computer Science Laboratories
  Author(s) Peter Hanappe

  romi-rover is collection of applications for the Romi Rover.

  romi-rover is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see
  <http://www.gnu.org/licenses/>.

 */

#include <stdexcept>
#include <string.h>
#include "RPCServer.h"

namespace rcom {
        
        RPCServer::RPCServer(IRPCHandler& handler,
                             const char *name,
                             const char *topic)
                : handler_(handler), hub_(topic, *this)
        {
                (void) name;
        }

        static int32_t to_membuffer(void* userdata, const char* s, size_t len)
        {
                rpp::MemBuffer *serialised = reinterpret_cast<rpp::MemBuffer*>(userdata);
                serialised->append((uint8_t *) s, len);
                return 0;
        }
        
        void RPCServer::onmessage(IWebSocket& websocket, rpp::MemBuffer& message)
        {
                (void) message;
                JsonCpp request;
                JsonCpp result;
                JsonCpp params;
                RPCError error;
                json_object_t response = json_null();
                
                try {
                        error.code = 0;
                        
                        JsonCpp request = JsonCpp::parse(message);
                        
                        const char *method = request.str("method");
                        if (request.has("params"))
                                params = request.get("params");
                        
                        handler_.execute(method, params, result, error);
                        
                        response = construct_response(error, result);
                        
                } catch (std::exception& e) {
                        response = construct_response(RPCError::kInternalError, e.what());
                }

                // FIXME! improve JsonCpp and/or MemBuffer
                // Yet another copy
                rpp::MemBuffer serialised;
                json_serialise(response, k_json_compact, to_membuffer,
                               reinterpret_cast<void*>(&serialised));
                websocket.send(serialised);
        }
        
        /* Construct the envelope for a reponse with results, to be
         * sent back to the client. This is still done "the old way",
         * for now, using the C JsonCpp API.  */
        json_object_t RPCServer::construct_response(RPCError& error, JsonCpp& result)
        {
                json_object_t response = construct_response(error.code,
                                                            error.message.c_str());
                
                if (!result.isnull()) {
                        json_object_set(response, "result", result.ptr());
                }
                
                return response;
        }

        /* Construct the envelope for an error reponse to be sent back
         * to the client. This is still done "the old way", for now,
         * using the C JsonCpp API.  */
        json_object_t RPCServer::construct_response(int code, const char *message)
        {
                json_object_t response = json_object_create();
                
                if (code != 0) {
                        json_object_t error = json_object_create();
                        json_object_set(response, "error", error);
                        json_object_setnum(error, "code", code);
                        
                        if (message != nullptr && strlen(message) > 0) {
                                json_object_setstr(error, "message", message);
                        } else {
                                json_object_setstr(error, "message",
                                                   "No message was given");
                        }
                        
                        json_unref(error); // refcount held by response object
                }

                return response;
        }

        void RPCServer::handle_events()
        {
                hub_.handle_events();
        }
}
