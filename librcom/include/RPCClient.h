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
#ifndef _LIBRCOM_RPC_CLIENT_H_
#define _LIBRCOM_RPC_CLIENT_H_

#include "IRPCHandler.h"
#include "MessageLink.h"

namespace rcom {
        
        class RPCClient : public IRPCHandler
        {
        protected:
                MessageLink link_;
                rpp::MemBuffer buffer_;

                void try_execute(const std::string& method, JsonCpp &params,
                                 JsonCpp &result, RPCError &error);
                void make_request(const std::string& method, JsonCpp &params);
                bool send_request(RPCError &error);
                void receive_response(JsonCpp &result, RPCError &error);
                void parse_response(JsonCpp &result, RPCError &error);
                void set_error(RPCError &error);
                        
        public:
                RPCClient(const char *name, const char *topic, double timeout_seconds);
                ~RPCClient() override = default;

                /** execute() does not throw exceptions. All errors
                 * are returned through the RPCError structure. */
                void execute(const std::string& method, JsonCpp &params,
                             JsonCpp &result, RPCError &error) override;
        };
}

#endif // _LIBRCOM_RPC_CLIENT_H_
