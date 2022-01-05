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
#include "ConsoleLogger.h"
#include "MessageHub.h"
#include "WebSocketServer.h"
#include "util.h"
#include "DummyMessageListener.h"

namespace rcom {

    MessageHub::MessageHub(const std::string& topic,
                           const std::shared_ptr<IMessageListener>& listener,
                           const std::shared_ptr<ISocketFactory>& socketFactory,
                           const std::shared_ptr<IWebSocketServerFactory>& webSocketServerFactory)
                           : RawMessageHub(topic, listener, socketFactory, webSocketServerFactory,0)

    {
        if (!register_topic()) {
            log_error("MessageHub: Registration failed: topic '%s'", topic.c_str());
            throw std::runtime_error("MessageHub: Registration failed");
        }
    }

    MessageHub::MessageHub(const std::string& topic,
                           const std::shared_ptr<ISocketFactory>& socketFactory,
                           const std::shared_ptr<IWebSocketServerFactory>& webSocketServerFactory)
            : MessageHub(topic, std::make_shared<DummyMessageListener>(), socketFactory, webSocketServerFactory)
    {
    }

}
