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
#include "RPCClient.h"

namespace rcom {
        
        RPCClient::RPCClient(const char *name, const char *topic, double timeout_seconds)
        {
                (void) name;
                (void) topic;
                (void) timeout_seconds;
                throw std::runtime_error("RPCClient::RPCClient: Not implemented");
        }

        void RPCClient::execute(const std::string& method, JsonCpp &params,
                                JsonCpp &result, RPCError &error)
        {
                (void) method;
                (void) params;
                (void) result;
                error.code = RPCError::kInternalError;
                error.message = "Not implemented";
        }
}
