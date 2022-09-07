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
#include "rcom/IMessageLink.h"
#include "rcom/ISocketFactory.h"
#include "rcom/IWebSocket.h"
#include "rcom/ILog.h"
#include "rcom/Address.h"

namespace rcom {

        class MessageLink : public IMessageLink
        {
        protected:

                std::shared_ptr<ISocketFactory> factory_;
                std::unique_ptr<IWebSocket> websocket_;
                std::string topic_;
                RecvStatus recv_status_;
                std::shared_ptr<ILinux> linux_;
                std::shared_ptr<ILog> log_;
                
                bool connect(double timeout);
                bool get_remote_address(Address& address, double timeout);
                bool obtained_message();

        public:

                static std::unique_ptr<IMessageLink> create(const std::string& topic,
                                                            double timeout);
                static std::unique_ptr<IMessageLink> create(const std::string& topic,
                                                            double timeout,
                                                            const std::shared_ptr<ILog>& log);
                
                MessageLink(const std::string& topic,
                            double timeout,
                            const std::shared_ptr<ISocketFactory>& factory,
                            const std::shared_ptr<ILinux>& linux,
                            const std::shared_ptr<ILog>& log);                
                virtual ~MessageLink();

                std::string& get_topic() override;                
                bool recv(MemBuffer& message, double timeout) override;
                bool send(MemBuffer& message,
                          MessageType type = kTextMessage) override;
                RecvStatus recv_status() override;
                bool is_connected() override;
        };
}

#endif // _LIBRCOM_MESSAGE_LINK_H_
