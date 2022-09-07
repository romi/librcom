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
#include "rcom/Log.h"
#include "rcom/RemoteStub.h"

namespace rcom {

        RemoteStub::RemoteStub(std::unique_ptr<IRPCClient>& client,
                               const std::shared_ptr<ILog>& log)
                : client_(),
                  log_(log)
        {
                client_ = std::move(client);
        }

        bool RemoteStub::execute(const std::string& method,
                                 nlohmann::json& params,
                                 nlohmann::json& result)
        {
                RPCError error;
                
                try {
                        client_->execute(method, params, result, error);

                        if (error.code != 0) {
                                log_err(log_, "RemoteStub::execute: %s",
                                        error.message.c_str());
                        }
                        
                } catch (std::exception& e) {
                        
                        log_err(log_, "RemoteStub::execute: '%s'", e.what());
                        error.code = 1;
                        error.message = e.what();
                }
                
                return (error.code == 0);
        }

        bool RemoteStub::execute_with_result(const std::string& method,
                                             nlohmann::json& result)
        {
                nlohmann::json params;
                return execute(method, params, result);
        }

        bool RemoteStub::execute_with_params(const std::string& method,
                                             nlohmann::json& params)
        {
                nlohmann::json result;
                return execute(method, params, result);
        }

        bool RemoteStub::execute_simple_request(const std::string& method)
        {
                nlohmann::json params;
                nlohmann::json result;
                return execute(method, params, result);
        }
}
