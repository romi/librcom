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
#include "rcom/Frames.h"
#include "rcom/Log.h"
#include "rcom/ServerSideWebSocket.h"

namespace rcom {
        
        ServerSideWebSocket::ServerSideWebSocket(std::unique_ptr<ISocket>& socket,
                                                 IRequestParser& parser,
                                                 const std::shared_ptr<ILinux>& linux,
                                                 const std::shared_ptr<ILog>& log)
                : WebSocket(socket, linux, log)
        {
                try {
                        handshake(parser);
                        
                } catch (std::exception& e) {
                        log_err(log_, "ServerSideWebSocket::handhake failed");
                        throw;
                }
        }

        void ServerSideWebSocket::handshake(IRequestParser& parser)
        {
                parser.parse(*socket_);
                IRequest& request = parser.request();
                request.assert_websocket();
                upgrade_connection(request);
        }

        void ServerSideWebSocket::upgrade_connection(IRequest& request)
        {
                std::string key;
                std::string accept;
                MemBuffer response;
                        
                request.get_header_value("Sec-WebSocket-Key", key);
                make_accept_string(accept, key);
                make_http_response(response, accept);
                
                if (!socket_send(response)) {
                        throw std::runtime_error("ServerSideWebSocket::upgrade_connection"
                                                 " send failed");
                }
        }
        
        void ServerSideWebSocket::make_http_response(MemBuffer& response,
                                                     std::string &accept)
        {
                response.printf("HTTP/1.1 101 Switching Protocols\r\n"
                                "Upgrade: websocket\r\n"
                                "Connection: Upgrade\r\n"
                                "Sec-WebSocket-Accept: %s\r\n"
                                "\r\n", accept.c_str());
        }

        void ServerSideWebSocket::input_assert_mask_flag()
        {
                // https://tools.ietf.org/html/rfc6455#section-5.1
                //
                // "... a client MUST mask all frames that it sends to the
                // server [...].  The server MUST close the connection upon
                // receiving a frame that is not masked.  In this case, a
                // server MAY send a Close frame with a status code of 1002
                // (protocol error) 
                //
                // 5.2: Mask: 1 bit: ... All frames sent from client to server
                // have this bit set to 1.

                if (!frame_header_.mask) {
                        log_err(log_, "ServerSideWebSocket: The client sent an "
                                "unmasked frame.");
                        throw RecvError("Client sent unmasked frame", kCloseProtocolError);
                }
        }

        void ServerSideWebSocket::close_connection()
        {
                socket_->close();
        }

        void ServerSideWebSocket::input_read_payload_mask()
        {
                socket_read(input_mask_, 4);
        }

        void ServerSideWebSocket::input_read_header()
        {
                input_read_frame_header();
                input_read_payload_length();
                input_read_payload_mask();
        }

        void ServerSideWebSocket::input_append_payload(uint8_t *data, size_t length)
        {
                unmask_data(data, data, length);
                input_payload_buffer_.append(data, length);
        }

        void ServerSideWebSocket::unmask_data(uint8_t *out, const uint8_t *in,
                                              size_t length)
        {
                apply_mask(out, in, length, input_mask_);
        }

        void ServerSideWebSocket::output_append_header(MemBuffer& output,
                                                       Opcode opcode,
                                                       MemBuffer& message)
        {
                size_t length = message.size();
                uint8_t frame[14];
                size_t n = 0;
        
                frame[n++] = (uint8_t) (0x80 | opcode);
        
                if (length < Frames::data_length_126) {
                        frame[n++] = (uint8_t) (length & 0x7f);
                
                } else if (length <= Frames::data_length_65536) {
                        uint16_t netshort = htons((uint16_t)length);
                        frame[n++] = Frames::frame_type_server_126;
                        frame[n++] = (uint8_t) (netshort & 0x00ff);
                        frame[n++] = (uint8_t) ((netshort & 0xff00) >> 8);
                
                } else {
                        uint64_t hostlong = length; 
                        uint64_t netlong = htonll(hostlong);
                        frame[n++] = Frames::frame_type_server_127;
                        frame[n++] = (uint8_t) (netlong & 0x00000000000000ff);
                        frame[n++] = (uint8_t) ((netlong & 0x000000000000ff00) >> 8);
                        frame[n++] = (uint8_t) ((netlong & 0x0000000000ff0000) >> 16);
                        frame[n++] = (uint8_t) ((netlong & 0x00000000ff000000) >> 24);
                        frame[n++] = (uint8_t) ((netlong & 0x000000ff00000000) >> 32);
                        frame[n++] = (uint8_t) ((netlong & 0x0000ff0000000000) >> 40);
                        frame[n++] = (uint8_t) ((netlong & 0x00ff000000000000) >> 48);                
                        frame[n++] = (uint8_t) ((netlong & 0xff00000000000000) >> 56);
                }

                output.append(frame, n);
        }

        void ServerSideWebSocket::output_append_payload(MemBuffer& output,
                                                        MemBuffer& message)
        {
                output.append(message);
        }

        bool ServerSideWebSocket::output_send_payload(const uint8_t *data, size_t length)
        {
                return socket_send(data, length);
        }
}
