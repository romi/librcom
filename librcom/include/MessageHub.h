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
#include "RawMessageHub.h"
#include "IWebSocketServer.h"
#include "IMessageListener.h"

namespace rcom {

        class MessageHub : public RawMessageHub
        {
        public:
                
                MessageHub(const std::string& topic,
                           const std::shared_ptr<IMessageListener>& listener,
                           const std::shared_ptr<IWebSocketServerFactory>& webSocketServerFactory);

                /* This constructor is used for publisher-subscriber
                 * patterns in which the message hub does not expect
                 * to receive any messages from the subscribers. */
                /* Should really be a different class if it's not a HUB. REFACTOR */
                explicit MessageHub(const std::string& topic,
                                    const std::shared_ptr<IWebSocketServerFactory>& webSocketServerFactory);
                ~MessageHub() override = default;
        };
}

#endif // _LIBRCOM_MESSAGE_HUB_H_

