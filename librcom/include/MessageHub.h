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
#ifndef _LIBRCOM_MESSAGE_HUB_H_
#define _LIBRCOM_MESSAGE_HUB_H_

#include <memory>
#include <Linux.h>
#include "SocketFactory.h"
#include "Clock.h"
#include "IMessageHub.h"
#include "IWebSocketServer.h"
#include "IMessageListener.h"

namespace rcom {

        class MessageHub : public IMessageHub, public IMessageListener
        {
        protected:
                rpp::Linux linux_;
                Clock clock_;
                SocketFactory factory_;
                std::unique_ptr<IWebSocketServer> server_;
                std::string topic_;
                
                bool register_topic();
                
        public:
                
                MessageHub(const std::string& topic,
                           IMessageListener& listener);

                /* This constructor is used for publisher-subscriber
                 * patterns in which the message hub does not expect
                 * to receive any messages from the subscribers. */
                MessageHub(const std::string& topic);
                virtual ~MessageHub(); 

                std::string& topic() override;
                void handle_events() override;
                void broadcast(rpp::MemBuffer& message,
                               IWebSocket* exclude = nullptr,
                               MessageType type = kTextMessage) override;

                void onmessage(IWebSocket& link, rpp::MemBuffer& message) override;
        };
}

#endif // _LIBRCOM_MESSAGE_HUB_H_

