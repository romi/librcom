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
#ifndef _LIBRCOM_REMOTESTUB_H
#define _LIBRCOM_REMOTESTUB_H

#include <memory>
#include <utility>
#include "rcom/IRPCClient.h"
#include "rcom/ILog.h"

namespace rcom {
        
        class RemoteStub
        {
        protected:
                std::unique_ptr<IRPCClient> client_;
                std::shared_ptr<ILog> log_;

                bool execute(const std::string& method, nlohmann::json& params,
                             nlohmann::json& result);
                bool execute_with_params(const std::string& method,
                                         nlohmann::json& params);
                bool execute_with_result(const std::string& method,
                                         nlohmann::json& result);
                bool execute_simple_request(const std::string& method);

        public:
                explicit RemoteStub(std::unique_ptr<IRPCClient>& client);
                virtual ~RemoteStub() = default;
        };
}

#endif // _LIBRCOM_REMOTESTUB_H
