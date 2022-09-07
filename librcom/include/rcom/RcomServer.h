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
#ifndef _LIBRCOM_RCOMSERVER_H
#define _LIBRCOM_RCOMSERVER_H

#include <memory>
#include "rcom/IMessageHub.h"
#include "rcom/IRPCServer.h"
#include "rcom/IRPCHandler.h"
#include "rcom/ILog.h"

namespace rcom {
        
        class RcomServer : public IRPCServer
        {
        protected:
                std::unique_ptr<IMessageHub> hub_;

        public:
                
                static std::unique_ptr<IRPCServer> create(const std::string& topic,
                                                          IRPCHandler &handler);
                
                static std::unique_ptr<IRPCServer> create(const std::string& topic,
                                                          IRPCHandler &handler,
                                                          const std::shared_ptr<ILog>& log);
                
                RcomServer(std::unique_ptr<IMessageHub>& hub);
                virtual ~RcomServer() = default;

                void handle_events();
        };
}

#endif // _LIBRCOM_RCOMSERVER_H
