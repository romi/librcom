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
#ifndef _LIBRCOM_REMOTEOBJECTSADAPTOR_H
#define _LIBRCOM_REMOTEOBJECTSADAPTOR_H

#include "rcom/IRPCHandler.h"

namespace rcom {

        using ObjectMap = std::map<std::string, std::shared_ptr<rcom::IRPCHandler>>;
        
        class RemoteObjectsAdaptor : public rcom::IRPCHandler
        {
        protected:

                static const constexpr char *kObjectID = "object-id";
                
                ObjectMap map_;

                rcom::IRPCHandler *get_adaptor(const std::string& id);
                
        public:
                RemoteObjectsAdaptor();
                ~RemoteObjectsAdaptor() override = default;

                void add(const std::string& id,
                         std::shared_ptr<rcom::IRPCHandler>& adaptor);
                
                void execute(const std::string& id,
                             const std::string& method,
                             nlohmann::json& params,
                             nlohmann::json& result,
                             rcom::RPCError& error) override;
                void execute(const std::string& id,
                             const std::string& method,
                             nlohmann::json& params,
                             rcom::MemBuffer& result,
                             rcom::RPCError &error) override;
        };
}

#endif // _LIBRCOM_REMOTEOBJECTSADAPTOR_H
