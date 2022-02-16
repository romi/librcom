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
#ifndef _LIBRCOM_RAWMESSAGE_HUB_H_
#define _LIBRCOM_RAWMESSAGE_HUB_H_

#include <memory>
#include <Linux.h>
#include "SocketFactory.h"
#include "IMessageHub.h"
#include "IWebSocketServerFactory.h"
#include "IMessageListener.h"

namespace rcom {

        class RawMessageHub : public IMessageHub
        {
        protected:
                std::unique_ptr<IWebSocketServer> server_;
                const std::shared_ptr<ISocketFactory> socketFactory_;
                std::string topic_;
                
                bool register_topic();

        public:

            RawMessageHub(const std::string& topic,
                            const std::shared_ptr<IMessageListener>& listener,
                            const std::shared_ptr<ISocketFactory>& socketFactory,
                            const std::shared_ptr<IWebSocketServerFactory>& webSocketServerFactory);

            RawMessageHub(const std::string& topic,
                            const std::shared_ptr<IMessageListener>& listener,
                            const std::shared_ptr<ISocketFactory>& socketFactory,
                            const std::shared_ptr<IWebSocketServerFactory>& webSocketServerFactory,
                            uint16_t port);

            ~RawMessageHub() override = default;

            std::string& topic() override;
            void handle_events() override;
            void broadcast(rcom::MemBuffer &message, MessageType type,
                           IWebSocket *exclude) override;
            size_t count_links() override;
        };
}

#endif

