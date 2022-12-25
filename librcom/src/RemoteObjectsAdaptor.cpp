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
#include <iostream>
#include "rcom/RemoteObjectsAdaptor.h"

namespace rcom {

        RemoteObjectsAdaptor::RemoteObjectsAdaptor()
                : map_()
        {
        }
        
        void RemoteObjectsAdaptor::execute(const std::string& id,
                                           const std::string& method,
                                           nlohmann::json& params,
                                           nlohmann::json& result,
                                           RPCError& error)
        {
                IRPCHandler *adaptor = get_adaptor(id);

                if (adaptor != nullptr) {
                        adaptor->execute(id, method, params, result, error);
                                
                } else {
                        error.code = 2;
                        error.message = "Unknown object id";
                }
        }
        
        void RemoteObjectsAdaptor::execute(const std::string& id,
                                           const std::string& method,
                                           nlohmann::json& params,
                                           MemBuffer& result,
                                           RPCError &error)
        {
                IRPCHandler *adaptor = get_adaptor(id);

                if (adaptor != nullptr) {
                        adaptor->execute(id, method, params, result, error);
                                
                } else {
                        error.code = 2;
                        error.message = "Unknown object id";
                }
        }

        IRPCHandler *RemoteObjectsAdaptor::get_adaptor(const std::string& id)
        {
                IRPCHandler *result = nullptr;
                ObjectMap::iterator it;
                
                it = map_.find(id);
                if (it != map_.end()) {
                        result = it->second.get();
                }
                
                return result;
        }

        void RemoteObjectsAdaptor::add(const std::string& id,
                                       std::shared_ptr<IRPCHandler>& ptr)
        {
                map_.insert(std::pair<std::string, std::shared_ptr<IRPCHandler>>(id, ptr));
        }
}

