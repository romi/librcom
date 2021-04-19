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
#ifndef _LIBRCOM_MESSAGE_LINK_H_
#define _LIBRCOM_MESSAGE_LINK_H_

#include <memory>
#include <Linux.h>
#include "IMessageLink.h"
#include "Clock.h"
#include "SocketFactory.h"
#include "IWebSocket.h"
#include "Address.h"

namespace rcom {

        class MessageLink : public IMessageLink
        {
        protected:

                rpp::Linux linux_;
                Clock clock_;
                SocketFactory factory_;
                std::unique_ptr<IWebSocket> websocket_;
                std::string topic_;
                RecvStatus recv_status_;
                
                bool connect(double timeout);
                bool get_remote_address(Address& address, double timeout);
                bool obtained_message();

        public:
                
                MessageLink(const std::string& topic, double timeout = 7.0);                
                virtual ~MessageLink();

                std::string& get_topic() override;                
                bool recv(rpp::MemBuffer& message, double timeout = 0.0) override;
                bool send(rpp::MemBuffer& message) override;
                RecvStatus recv_status() override;
        };
}

#endif // _LIBRCOM_MESSAGE_LINK_H_
