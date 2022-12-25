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
#include "rcom/Log.h"

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
                try {
                        IRPCHandler& adaptor = get_adaptor(id);
                        adaptor.execute(id, method, params, result, error);
                } catch (std::exception& e) {
                        error.code = 2;
                        error.message = e.what();
                }
        }
        
        void RemoteObjectsAdaptor::execute(const std::string& id,
                                           const std::string& method,
                                           nlohmann::json& params,
                                           MemBuffer& result,
                                           RPCError &error)
        {
                try {
                        IRPCHandler& adaptor = get_adaptor(id);
                        adaptor.execute(id, method, params, result, error);
                } catch (std::exception& e) {
                        error.code = 2;
                        error.message = e.what();
                }
        }

        IRPCHandler& RemoteObjectsAdaptor::get_adaptor(const std::string& id)
        {
                ObjectMap::iterator it = map_.find(id);
                if (it != map_.end()) {
                        return it->second;
                } else {
                        throw std::runtime_error("Can't find object with given id");
                }
        }

        void RemoteObjectsAdaptor::add(const std::string& id, IRPCHandler& ptr)
        {
                map_.insert(std::pair<std::string, IRPCHandler&>(id, ptr));
        }
}

