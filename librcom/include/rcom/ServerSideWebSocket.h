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
#ifndef _LIBRCOM_SERVERSIDEWEBSOCKET_H_
#define _LIBRCOM_SERVERSIDEWEBSOCKET_H_

#include "rcom/WebSocket.h"
#include "rcom/IRequestParser.h"

namespace rcom {

        class ServerSideWebSocket : public WebSocket
        {
        protected:
                uint8_t input_mask_[4];

                void handshake(IRequestParser& parser);
                void upgrade_connection(IRequest& request);
                void make_http_response(MemBuffer& response, std::string &accept);
                void unmask_data(uint8_t *out, const uint8_t *in, size_t length);
                void input_read_payload_mask();
                
                void input_assert_mask_flag() override;
                void input_read_header() override;
                void input_append_payload(uint8_t *data, size_t length) override;
                void output_append_header(MemBuffer& output,
                                          Opcode opcode,
                                          MemBuffer& message) override;
                void output_append_payload(MemBuffer& output,
                                           MemBuffer& message) override;
                bool output_send_payload(const uint8_t *data, size_t length) override;
                void close_connection() override;

                
        public:
                
                ServerSideWebSocket(std::unique_ptr<ISocket>& socket,
                                    IRequestParser& parser,
                                    const std::shared_ptr<ILinux>& linux,
                                    const std::shared_ptr<ILog>& log);
                
                ~ServerSideWebSocket() override = default;
        };
}

#endif // _LIBRCOM_SERVERSIDEWEBSOCKET_H_
