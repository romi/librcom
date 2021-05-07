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
#ifndef _LIBRCOM_WEBSOCKET_H_
#define _LIBRCOM_WEBSOCKET_H_

#include <memory>
#include "ISocket.h"
#include "IWebSocket.h"

#include <endian.h>
#define htonll(_x) htobe64(_x)
#define ntohll(_x) be64toh(_x)


namespace rcom {

        class RecvError : public std::exception
        {
        protected:
                std::string what_;
                CloseCode code_;
                
        public:
                RecvError(const char *what, CloseCode code)
                        : std::exception(), what_(what), code_(code)
                        {}
                
                const char* what() const noexcept override {
                        return what_.c_str(); 
                }
                
                CloseCode code() const noexcept {
                        return code_;
                }
        };

        
        struct FrameHeader {
                bool fin;
                uint8_t opcode;
                bool mask;
                uint8_t length;
        };

        
        class WebSocket : public IWebSocket
        {
        public:
                
                /* The maximum length of the payload of a single
                 * message. This maximum length is an implementation
                 * choice, not a limit specified by the websocket
                 * protocol (RFC 6455).  */
                static const uint64_t kMaximumPayloadLength = (128*1024*1024);
                
                /* The maximum length of a message, including
                 * coalesced fragmented messages. This maximum length
                 * is an implementation choice, not a limit specified
                 * by the websocket protocol (RFC 6455). */
                static const uint64_t kMaximumMessageLength = (128*1024*1024);
                
                static const uint64_t kShortMessageLength = (128*1024);

                static const size_t kPayloadBufferLength = 1024;
                
        protected:
                
                std::unique_ptr<ISocket> socket_;

                // Used by send()
                rpp::MemBuffer output_message_buffer_;

                // Used by recv()
                rpp::MemBuffer input_payload_buffer_;
                FrameHeader frame_header_;
                bool is_text_;
                bool is_continuation_;
                CloseCode remote_close_reason_;
                uint64_t input_message_length_;
                
        public:
                
                WebSocket(std::unique_ptr<ISocket>& socket);
                virtual ~WebSocket() override;

                RecvStatus recv(rpp::MemBuffer& message, double timeout = 0.0);
                bool send(rpp::MemBuffer& message, MessageType type = kTextMessage);
                void close(CloseCode code) override;
                bool is_connected() override;

        protected:

                // Implementation shared by client or server sub-classes
                RecvStatus try_recv(rpp::MemBuffer& message, double timeout);
                bool wait_and_handle_one_message(rpp::MemBuffer& message, double timeout);
                bool wait_for_data(double timeout);
                WaitStatus socket_wait(double timeout);
                void handle_one_message(rpp::MemBuffer& message);
                void read_message();
                void input_read_frame_header();
                RecvStatus get_message_type();
                void input_read_payload_length();
                void input_read_uint16_payload_length();
                void input_read_uint64_payload_length();
                uint16_t input_read_uint16();
                uint64_t input_read_uint64();
                void input_assert_valid_message();
                void input_assert_opcode();
                void input_assert_payload_length();
                void input_read_payload();
                void process_message(rpp::MemBuffer& message);
                bool has_control_message();
                bool has_data_message();
                bool has_complete_data_message();
                bool is_data_message_complete();
                bool has_unfragmented_text_message();
                bool has_unfragmented_binary_message();
                bool has_final_continuation_message();
                void handle_control_message();
                void answer_close_handshake();
                void get_close_code();
                void closing_send_reply();
                bool send_close_message(CloseCode code);
                void close_without_handshake();
                void try_close_with_handshake(CloseCode code);
                void close_with_handshake(CloseCode code);
                void closing_wait_reply(double timeout);
                void send_pong();
                void handle_pong_response();
                void handle_data_payload(rpp::MemBuffer& message);
                void assert_continuation();
                void assert_message_length(rpp::MemBuffer& message);
                void set_payload_type();
                void copy_payload(rpp::MemBuffer& message);
                void prepare_continuation();
                bool send_short_data_message(rpp::MemBuffer& message, MessageType type);
                bool send_long_data_message(rpp::MemBuffer& message, MessageType type);
                bool send_long_message_header(Opcode opcode, rpp::MemBuffer& message);
                bool send_long_message_payload(rpp::MemBuffer& message);
                void make_message(Opcode opcode, rpp::MemBuffer& message);
                void make_message(rpp::MemBuffer& output, Opcode opcode,
                                  rpp::MemBuffer& message);
                void turn_buffering_off();
                void turn_buffering_on();
                bool send_output_buffer();
                bool socket_send(rpp::MemBuffer& buffer);
                bool socket_send(const uint8_t *buffer, size_t length);
                void socket_read(uint8_t *buffer, size_t length);
                double compute_remaning_time(double start_time, double timeout);

                // Utility functions
                static void make_accept_string(std::string& accept, std::string& key);
                static void apply_mask(uint8_t *out, const uint8_t *in,
                                       size_t length, uint8_t *mask);
                
                // To be implemented by client or server sub-classes
                virtual void input_assert_mask_flag() = 0;
                virtual void input_read_header() = 0;
                virtual void input_append_payload(uint8_t *data, size_t length) = 0;
                
                virtual void output_append_header(rpp::MemBuffer& output,
                                                  Opcode opcode,
                                                  rpp::MemBuffer& message) = 0;
                virtual void output_append_payload(rpp::MemBuffer& output,
                                                   rpp::MemBuffer& message) = 0;
                virtual bool output_send_payload(const uint8_t *data, size_t length) = 0;
                virtual void close_connection() = 0;
        };
}



        

#endif // _LIBRCOM_WEBSOCKET_H_
