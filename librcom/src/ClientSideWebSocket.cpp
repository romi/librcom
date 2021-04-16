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
#include <r.h>
#include "ClientSideWebSocket.h"
#include "IClock.h"
#include "util.h"

namespace rcom {

        ClientSideWebSocket::ClientSideWebSocket(std::unique_ptr<ISocket>& socket,
                                                 IResponseParser& parser,
                                                 IAddress& remote_address,
                                                 IClock& clock)
                : WebSocket(socket, clock)
        {
                if (!handshake(parser, remote_address)) {
                        r_err("ClientSideWebSocket::handhake failed");
                        throw std::runtime_error("ClientSideWebSocket: Handshake failed");
                }
        }
        
        ClientSideWebSocket::~ClientSideWebSocket()
        {
        }

        void ClientSideWebSocket::input_assert_mask_flag()
        {
                // https://tools.ietf.org/html/rfc6455#section-5.1
                //
                // "A server MUST NOT mask any frames that it sends to
                // the client.  A client MUST close a connection if it
                // detects a masked frame.  In this case, it MAY use
                // the status code 1002 (protocol error)"

                if (frame_header_.mask) {
                        r_err("ClientSideWebSocket: The server sent a masked frame.");
                        throw RecvError("Server sent masked frame", kCloseProtocolError);
                }
        }
        
        bool ClientSideWebSocket::handshake(IResponseParser& parser, IAddress& address)
        {
                bool success = false;
                std::string key;
                std::string accept;
                std::string host;
                
                make_key(key);
                make_accept_string(accept, key);
                address.tostring(host);
        
                if (send_http_request(host, key)) {
                        if (parser.parse(*socket_)) {
                                if (parser.response().is_websocket(accept)) {
                                        success = true;
                                } else {
                                        r_err("ClientSideWebSocket::handshake: "
                                              "invalid response");
                                }
                        } else {
                                r_err("ClientSideWebSocket::handshake: "
                                      "failed to parse the response");
                        }
                } else {
                        r_err("ClientSideWebSocket::handshake: "
                              "failed to send the request");
                }
        
                return success;
        }
        
        void ClientSideWebSocket::make_key(std::string& key)
        {
                /* RFC 6455, 4.1.  Client Requirements
                   The request MUST include a header field with the name
                   |Sec-WebSocket-Key|.  The value of this header field MUST be a
                   nonce consisting of a randomly selected 16-byte value that has
                   been base64-encoded (see Section 4 of [RFC4648]).  The nonce
                   MUST be selected randomly for each connection.
                */                            
                uint8_t bytes[17];
                memset(bytes, 0, sizeof(bytes));
                r_random(bytes, 16);
                encode_base64(bytes, 16, key);
        }

        bool ClientSideWebSocket::send_http_request(std::string& host, std::string& key)
        {
                rpp::MemBuffer request;
                make_http_request(request, host, key);
                // r_debug("ClientSideWebSocket::send_http_request: %s",
                //         request.tostring().c_str());
                return socket_send(request);
        }
        
        void ClientSideWebSocket::make_http_request(rpp::MemBuffer& request, std::string& host,
                                                    std::string& key)
        {
                request.printf("GET / HTTP/1.1\r\n"
                               "Host: %s\r\n"
                               "Connection: Upgrade\r\n"
                               "Upgrade: websocket\r\n"
                               "Sec-WebSocket-Version: 13\r\n"
                               "Sec-WebSocket-Key: %s\r\n"
                               "\r\n",
                               host.c_str(), key.c_str());
        }

        void ClientSideWebSocket::close_connection()
        {
                /* RFC 6455, 7.1.1: "The underlying TCP connection, in
                   most normal cases, SHOULD be closed first by the
                   server, so that it holds the TIME_WAIT state and
                   not the client" */
                clock_.sleep(0.5);        
                socket_->close();
        }

        void ClientSideWebSocket::input_read_header()
        {
                input_read_frame_header();
                input_read_payload_length();
        }

        void ClientSideWebSocket::input_append_payload(uint8_t *data, size_t length)
        {
                input_payload_buffer_.append(data, length);
        }

        void ClientSideWebSocket::make_mask()
        {
                r_random(output_mask_, 4);
        }

        void ClientSideWebSocket::mask_data(uint8_t *out, const uint8_t *in, size_t length)
        {
                apply_mask(out, in, length, output_mask_);
        }

        void ClientSideWebSocket::output_append_header(rpp::MemBuffer& output,
                                                       Opcode opcode,
                                                       rpp::MemBuffer& message)
        {
                uint64_t length = message.size();
                uint8_t frame[14];
                size_t n = 0;
                
                frame[n++] = (uint8_t) (0x80 | opcode);
        
                if (length < 126) {
                        frame[n++] = (uint8_t) (0x80 | (length & 0x7f));
                
                } else if (length <= 65536) {
                        uint16_t netshort = htons((uint16_t)length);
                        frame[n++] = 254;
                        frame[n++] = (uint8_t) (netshort & 0x00ff);
                        frame[n++] = (uint8_t) ((netshort & 0xff00) >> 8);
                
                } else {
                        uint64_t netlong = htonll(length);
                        frame[n++] = 255;
                        frame[n++] = (uint8_t) (netlong & 0x00000000000000ff);
                        frame[n++] = (uint8_t) ((netlong & 0x000000000000ff00) >> 8);
                        frame[n++] = (uint8_t) ((netlong & 0x0000000000ff0000) >> 16);
                        frame[n++] = (uint8_t) ((netlong & 0x00000000ff000000) >> 24);
                        frame[n++] = (uint8_t) ((netlong & 0x000000ff00000000) >> 32);
                        frame[n++] = (uint8_t) ((netlong & 0x0000ff0000000000) >> 40);
                        frame[n++] = (uint8_t) ((netlong & 0x00ff000000000000) >> 48);                
                        frame[n++] = (uint8_t) ((netlong & 0xff00000000000000) >> 56);
                }

                make_mask();
                frame[n++] = output_mask_[0];
                frame[n++] = output_mask_[1];
                frame[n++] = output_mask_[2];
                frame[n++] = output_mask_[3];

                output.append(frame, n);
        }

        void ClientSideWebSocket::output_append_payload(rpp::MemBuffer& output,
                                                        rpp::MemBuffer& message)
        {
                uint8_t buffer[1024];
                size_t sent = 0;
                size_t length = message.size();
                const std::vector<uint8_t>& data = message.data();
                
                while (sent < length) {
                        uint64_t n = length - sent;
                        if (n > 1024)
                                n = 1024;

                        mask_data(buffer, &data[sent], n);
                
                        output.append(buffer, n);
                        sent += n;
                }
        }

        bool ClientSideWebSocket::output_send_payload(const uint8_t *data, size_t length)
        {
                uint8_t buffer[kPayloadBufferLength];
                mask_data(buffer, data, length);
                return socket_send(buffer, length);
        }
}
