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
#ifndef _LIBRCOM_MESSAGEHUB_H_
#define _LIBRCOM_MESSAGEHUB_H_

#include <memory>
#include "rcom/Linux.h"
#include "rcom/SocketFactory.h"
#include "rcom/IMessageHub.h"
#include "rcom/IWebSocketServer.h"
#include "rcom/IMessageListener.h"

namespace rcom {

        class MessageHub : public IMessageHub
        {
        protected:
                std::unique_ptr<IWebSocketServer> server_;
                std::shared_ptr<ISocketFactory> socket_factory_;
                std::string topic_;
                std::shared_ptr<ILinux> linux_;
                std::shared_ptr<ILog> log_;
                
        public:
                
                static std::unique_ptr<IMessageHub>
                create(const std::string& topic,
                       const std::shared_ptr<IMessageListener>& listener,
                       const std::shared_ptr<ILog>& log,
                       uint16_t port,
                       bool standalone);
                
                static std::unique_ptr<IMessageHub>
                create(const std::string& topic,
                       const std::shared_ptr<IMessageListener>& listener,
                       const std::shared_ptr<ILog>& log);

                static std::unique_ptr<IMessageHub>
                create(const std::string& topic,
                       const std::shared_ptr<IMessageListener>& listener);
                
                static std::unique_ptr<IMessageHub>
                create(const std::string& topic);
                
                static std::unique_ptr<IMessageHub>
                create(const std::string& topic,
                       const std::shared_ptr<ILog>& log);

                //
                
                MessageHub(const std::string& topic,
                           std::unique_ptr<IWebSocketServer>& server_socket,
                           const std::shared_ptr<ISocketFactory>& socket_factory,
                           const std::shared_ptr<ILinux>& linux,
                           const std::shared_ptr<ILog>& log);

                ~MessageHub() override = default;
                
                std::string& topic() override;
                void handle_events() override;
                void broadcast(MemBuffer &message, MessageType type,
                               IWebSocket *exclude) override;
                size_t count_links() override;
                void register_topic();
        };
}

#endif // _LIBRCOM_MESSAGEHUB_H_

