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
#ifndef _LIBRCOM_CLIENTSIDEWEBSOCKET_H_
#define _LIBRCOM_CLIENTSIDEWEBSOCKET_H_

#include "rcom/WebSocket.h"
#include "rcom/IResponseParser.h"
#include "rcom/ILinux.h"

namespace rcom {

        class ClientSideWebSocket : public WebSocket
        {
        protected:
                uint8_t output_mask_[4]{};

                void handshake(IResponseParser& parser, IAddress& address);
                void make_key(std::string& key);
                void send_http_request(std::string& host, std::string& key);
                void make_http_request(MemBuffer& request, std::string& host,
                                       std::string& key);
                void make_mask();
                void mask_data(uint8_t *out, const uint8_t *in, size_t length);
                
                
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
                
                ClientSideWebSocket(std::unique_ptr<ISocket>& socket,
                                    IResponseParser& parser,
                                    IAddress& remote_address,
                                    const std::shared_ptr<ILinux>& linux,
                                    const std::shared_ptr<ILog>& log);
                
                ~ClientSideWebSocket() override;
        };
}

#endif // _LIBRCOM_CLIENTSIDEWEBSOCKET_H_
