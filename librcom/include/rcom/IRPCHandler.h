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
#ifndef _LIBRCOM_I_RPCHANDLER_H
#define _LIBRCOM_I_RPCHANDLER_H

#include <string>
#include "rcom/json.hpp"
#include "rcom/MemBuffer.h"
#include "rcom/RPCError.h"

namespace rcom {
        
        class IRPCHandler
        {
        public:
                virtual ~IRPCHandler() = default;
                
                virtual void execute(const std::string& method,
                                     nlohmann::json &params,
                                     nlohmann::json &result,
                                     RPCError &status) = 0;
                
                virtual void execute(const std::string& method,
                                     nlohmann::json &params,
                                     MemBuffer& result,
                                     RPCError &status) = 0;
        };
}

#endif // _LIBRCOM_I_RPCHANDLER_H
