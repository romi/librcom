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
#ifndef _RCOM_WEBSOCKET_SERVER_H_
#define _RCOM_WEBSOCKET_SERVER_H_

#include <memory>
#include "IWebSocketServer.h"
#include "IAddress.h"
#include "IServerSocket.h"
#include "ISocketFactory.h"
#include "IMessageListener.h"

namespace rcom {
        
        class WebSocketServer : public IWebSocketServer
        {
        protected:
                std::unique_ptr<IServerSocket> server_socket_;
                ISocketFactory& factory_;
                IMessageListener& listener_;
                std::vector<std::unique_ptr<IWebSocket>> links_;
                rpp::MemBuffer message_;
                std::vector<size_t> to_close_;
                std::vector<size_t> to_remove_;
                
                void handle_new_connections();
                void try_new_connection(int sockfd);
                void handle_new_connection(int sockfd);
                void handle_new_messages();
                void handle_new_messages(size_t index);
                
                bool send(size_t index, rpp::MemBuffer& message,
                          MessageType type);
                void close(size_t index, CloseCode code);
                IWebSocket& append(int sockfd);
                void remove_closed_links();
                
        public:
                WebSocketServer(std::unique_ptr<IServerSocket>& server_socket,
                                ISocketFactory& factory,
                                IMessageListener& listener);
                virtual ~WebSocketServer();

                void handle_events() override;
                void broadcast(rpp::MemBuffer& message,
                               IWebSocket* exclude = nullptr,
                               MessageType type = kTextMessage) override;
                void get_address(IAddress& address) override;
                size_t count_links() override;
        };
}

#endif // _RCOM_WEBSOCKET_SERVER_H_
