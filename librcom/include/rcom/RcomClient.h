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
#ifndef _LIBRCOM_RCOMCLIENT_H
#define _LIBRCOM_RCOMCLIENT_H

#include <memory>
#include "rcom/IMessageLink.h"
#include "rcom/IRPCClient.h"
#include "rcom/ILog.h"

namespace rcom {
        
        class RcomClient : public IRPCClient
        {
        protected:
                std::unique_ptr<IMessageLink> link_;
                MemBuffer buffer_;
                double timeout_;
                
                void try_execute(const std::string& method, nlohmann::json &params,
                                 nlohmann::json &result, RPCError &error);
                void try_execute(const std::string& method, nlohmann::json &params,
                                 MemBuffer& result, RPCError &error);
                void make_request(const std::string& method, nlohmann::json &params);
                bool send_request(MessageType type, RPCError &error);
                bool receive_response(MemBuffer& buffer, RPCError &error);
                void parse_response(nlohmann::json &result, RPCError &error);
                void set_error(RPCError &error);
                        
        public:

                static std::unique_ptr<IRPCClient> create(const std::string& topic,
                                                          double timeout_seconds);
                static std::unique_ptr<IRPCClient> create(const std::string& topic,
                                                          double timeout_seconds,
                                                          const std::shared_ptr<ILog>& log);
                
                RcomClient(std::unique_ptr<IMessageLink>& link,
                           double timeout_seconds);
                ~RcomClient() override;

                /** execute() does not throw exceptions. All errors
                 * are returned through the RPCError structure. */
                void execute(const std::string& method,
                             nlohmann::json &params,
                             nlohmann::json &result,
                             RPCError &error) override;
                
                void execute(const std::string& method,
                             nlohmann::json &params,
                             MemBuffer& result,
                             RPCError &status) override;
                
                bool is_connected() override;
        };
}

#endif // _LIBRCOM_RCOMCLIENT_H
