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
#include <stdexcept>
#include <algorithm>
#include "rcom/ConsoleLogger.h"
#include "rcom/WebSocketServer.h"

namespace rcom {
        
        WebSocketServer::WebSocketServer(std::unique_ptr<IServerSocket>& server_socket,
                                         std::shared_ptr<ISocketFactory> factory,
                                         std::shared_ptr<IMessageListener> listener)
                : server_socket_(),
                  factory_(factory),
                  listener_(listener),
                  links_(),
                  message_(),
                  to_close_(),
                  to_remove_()
        {
                server_socket_ = std::move(server_socket);
        }
        
        WebSocketServer::~WebSocketServer()
        {
                server_socket_->close();
                for (size_t i = 0; i < links_.size(); i++) {
                        if (links_[i]->is_connected()) {
                                close(i, kCloseGoingAway);
                        }
                }
        }
        
        void WebSocketServer::get_address(IAddress& address)
        {
                server_socket_->get_address(address);
        }
        
        void WebSocketServer::handle_events()
        {
                handle_new_connections();
                handle_new_messages();
                remove_closed_links();
        }

        void WebSocketServer::handle_new_connections()
        {
                int sockfd = -1;
                do {
                        sockfd = server_socket_->accept(0.0);
                        if (sockfd >= 0) {
                                try_new_connection(sockfd);
                        }
                } while (sockfd >= 0);
        }
        
        void WebSocketServer::try_new_connection(int sockfd)
        {
                try {
                        handle_new_connection(sockfd);

                } catch (std::runtime_error& rerr) {
                        log_error("WebSocketServer::try_new_connection: %s", rerr.what());
                }
        }

        void WebSocketServer::handle_new_connection(int sockfd)
        {
                append(sockfd);
                // // lock
                // IWebSocket& websocket = append(sockfd);
                // listener_->onconnect(*this, websocket);
                // // unlock
        }

        void WebSocketServer::handle_new_messages()
        {
                for (size_t i = 0; i < links_.size(); i++) {
                        if (links_[i]->is_connected()) {
                                handle_new_messages(i);
                        }
                }
        }

        void WebSocketServer::remove_closed_links()
        {
                // lock
                links_.erase(std::remove_if(links_.begin(), 
                                            links_.end(),
                                            [](std::unique_ptr<IWebSocket> const& ws) {
//                                                    if (!ws->is_connected())
//                                                        log_info("remove_closed_links::removing ws");
                                                    return !ws->is_connected();
                                            }), 
                             links_.end()); 
                // unlock
        }
        
        void WebSocketServer::handle_new_messages(size_t index)
        {
                RecvStatus status;

                status = links_[index]->recv(message_);
                
                if (status == kRecvText
                    || status == kRecvBinary) {
                        MessageType type;
                        type = (status == kRecvText)? kTextMessage : kBinaryMessage;
                        listener_->onmessage(*links_[index], message_, type);
                                                
                } else if (status == kRecvError) {
                        log_error("WebSocketServer::handle_new_messages: recv failed. "
                              "Removing link.");
                        close(index, kCloseInternalError);
                        
                } else if (status == kRecvClosed) {
                    log_error("WebSocketServer::handle_new_messages: kRecvClose. Socket will be removed.");
                }
        }

        void WebSocketServer::broadcast(rcom::MemBuffer& message, MessageType type, IWebSocket *exclude)
        {
                for (size_t i = 0; i < links_.size(); i++) {
                        if (exclude != links_[i].get()) {
                                if (!send(i, message, type)) {
                                        log_warning("WebSocketServer::broadcast_text: "
                                               "send failed. Closing connection.");
                                        close(i, kCloseInternalError);
                                }
                        }
                }
        }

        bool WebSocketServer::send(size_t index, rcom::MemBuffer& message,
                                   MessageType type)
        {
                bool success = true;
                if (links_[index]->is_connected())
                        success = links_[index]->send(message, type);
                return success;
        }

        IWebSocket& WebSocketServer::append(int sockfd)
        {
                links_.emplace_back(factory_->new_server_side_websocket(sockfd));
                return *links_.back();
        }

        void WebSocketServer::close(size_t index, CloseCode code)
        {
                links_[index]->close(code);
        }

        size_t WebSocketServer::count_links()
        {
                return links_.size();
        }
        
        IWebSocket& WebSocketServer::get_link(size_t index)
        {
                return *links_[index];
        }
}
