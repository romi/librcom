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
#include <exception>
#include <string.h>
#include "ConsoleLogger.h"
#include "WebSocket.h"
#include "util.h"

namespace rcom {        

        static uint16_t convert_to_uint16(rcom::MemBuffer buffer);
        
        WebSocket::WebSocket(std::unique_ptr<ISocket>& socket)
                : socket_(),
                  output_message_buffer_(),
                  input_payload_buffer_(),
                  frame_header_{false,0,false,0},
                  is_text_(false),
                  is_continuation_(false),
                  remote_close_reason_(kCloseNormal),
                  input_message_length_(0)
        {
                socket_ = std::move(socket);
        }
        
        WebSocket::~WebSocket()
        {
        }

        ILinux& WebSocket::get_linux()
        {
                return socket_->get_linux();
        }

        void WebSocket::close(CloseCode code)
        {
                close_with_handshake(code);
        }

        bool WebSocket::is_connected()
        {
                return socket_->is_connected();
        }

        RecvStatus WebSocket::recv(rcom::MemBuffer& message, double timeout)
        {
                RecvStatus status = kRecvError;
                
                try {
                        message.clear();
                        status = try_recv(message, timeout);
                        
                } catch (RecvError& re) {
                        log_error("WebSocket::recv: %s", re.what());
                        close_with_handshake(re.code());
                }

                return status;
        }

        RecvStatus WebSocket::try_recv(rcom::MemBuffer& message, double timeout)
        {
                double start_time = rcom_time(get_linux());
                double remaining_time = timeout;
                RecvStatus status = kRecvTimeOut;
                
                while (remaining_time >= 0.0) {
                        
                        if (wait_and_handle_one_message(message, remaining_time)) {

                                if (has_complete_data_message()) {
                                        status = get_message_type();
                                        break;
                                }

                                if (!is_connected()) {
                                        status = kRecvClosed;
                                        break;
                                }
                        }
                        
                        remaining_time = compute_remaning_time(start_time, timeout);
                }
                
                return status;
        }
        
        double WebSocket::compute_remaning_time(double start_time, double timeout)
        {
                double now = rcom_time(get_linux());
                return timeout - (now - start_time);
        }
        
        bool WebSocket::wait_and_handle_one_message(rcom::MemBuffer& message, double timeout)
        {
                bool received_data = wait_for_data(timeout);
                if (received_data) 
                        handle_one_message(message);
                return received_data;
        }
        
        bool WebSocket::wait_for_data(double timeout)
        {
                bool success = false;
                WaitStatus status = socket_wait(timeout);
                if (status == kWaitOK) {
                        success = true;
                } else if (status == kWaitTimeout) {
                        success = false;
                } else {
                        throw RecvError("socket_wait failed",
                                        kCloseInternalError);
                }
                return success;
        }

        WaitStatus WebSocket::socket_wait(double timeout)
        {
                return socket_->wait(timeout);
        }
        
        RecvStatus WebSocket::get_message_type()
        {
                if (is_text_) {
                        return kRecvText;
                } else {
                        return kRecvBinary;
                }
        }
        
        void WebSocket::handle_one_message(rcom::MemBuffer& message)
        {
                read_message();
                process_message(message);
        }
        
        void WebSocket::read_message()
        {
                input_read_header();
                input_assert_valid_message();
                input_read_payload();
        }
        
        void WebSocket::input_read_frame_header()
        {
                uint8_t b[2];
                socket_read(b, 2);
                frame_header_.fin = (b[0] & (uint8_t) 0x80) != 0;
                frame_header_.opcode = (b[0] & (uint8_t) 0x0f);
                frame_header_.mask = (b[1] & (uint8_t) 0x80) != 0;
                frame_header_.length = (b[1] & (uint8_t) 0x7f);
        }

        void WebSocket::input_read_payload_length()
        {
                if (frame_header_.length < 126) {
                        input_message_length_ = (uint64_t) frame_header_.length;
                        
                } else if (frame_header_.length == 126) {
                        input_read_uint16_payload_length();
                        
                } else if (frame_header_.length == 127) {
                        input_read_uint64_payload_length();
                }
        }

        void WebSocket::input_read_uint16_payload_length()
        {
                input_message_length_ = input_read_uint16();
        }

        uint16_t WebSocket::input_read_uint16()
        {
                uint16_t netshort;                
                socket_read((uint8_t *) &netshort, 2);
                return ntohs(netshort);
        }

        void WebSocket::input_read_uint64_payload_length()
        {
                input_message_length_ = input_read_uint64();
        }
        
        uint64_t WebSocket::input_read_uint64()
        {
                uint64_t netlong;                
                socket_read((uint8_t *) &netlong, 8);
                return ntohll(netlong);
        }
        
        static uint16_t convert_to_uint16(rcom::MemBuffer buffer)
        {
                uint16_t value = 0;
                if (buffer.size() == 2) {
                        const std::vector<uint8_t>& data = buffer.data();
                        value = (uint16_t) (static_cast<unsigned>(data[0]) << 8
                                               | static_cast<unsigned>(data[1]));
                }
                return value;
        }

        void WebSocket::input_assert_valid_message()
        {
                input_assert_mask_flag();
                input_assert_opcode();
                input_assert_payload_length();
        }

        void WebSocket::input_assert_opcode()
        {
                bool valid = (frame_header_.opcode == kContinutationOpcode
                              || frame_header_.opcode == kTextOpcode
                              || frame_header_.opcode == kBinaryOpcode
                              || frame_header_.opcode == kCloseOpcode
                              || frame_header_.opcode == kPingOpcode
                              || frame_header_.opcode == kPongOpcode);
                if (!valid) {
                        log_error("WebSocket: Invalid opcode.");
                        throw RecvError("Invalid opcode", kCloseProtocolError);
                }
        }

        void WebSocket::input_assert_payload_length()
        {
                if (input_message_length_ > kMaximumPayloadLength) {
                        log_error("websocket_assert_payload_length: message too large "
                              "(%lu > %d MB)", input_message_length_,
                              (int) (kMaximumPayloadLength / (1024 * 1024)));
                        throw RecvError("Message too large", kCloseTooBig);
                }
        }
        
        void WebSocket::input_read_payload()
        {
                uint64_t length_processed = 0;
                uint64_t length_to_read;
                uint8_t buffer[kPayloadBufferLength];
        
                input_payload_buffer_.clear();
        
                while (length_processed < input_message_length_) {
                
                        length_to_read = input_message_length_ - length_processed;
                        if (length_to_read > kPayloadBufferLength)
                                length_to_read = kPayloadBufferLength;
                                
                        socket_read(buffer, (size_t) length_to_read);
                        
                        length_processed += length_to_read;
                        input_append_payload(buffer, (size_t) length_to_read);
                }
        }

        void WebSocket::socket_read(uint8_t *buffer, size_t length)
        {
                bool success = socket_->read(buffer, length);
                if (!success) {
                        log_error("WebSocket::socket_read: read failed");
                        throw RecvError("Read failed", kCloseInternalError);
                }
        }
        
        void WebSocket::process_message(rcom::MemBuffer& message)
        {
                if (has_control_message()) {
                        handle_control_message();
        
                } else if (has_data_message()) {
                        handle_data_payload(message);
                
                } else {
                        log_info("WebSocket: Invalid message opcode");
                        throw RecvError("Invalid opcode", kCloseProtocolError);
                }
        }

        bool WebSocket::has_control_message()
        {
                return (frame_header_.opcode == kCloseOpcode
                        || frame_header_.opcode == kPingOpcode
                        || frame_header_.opcode == kPongOpcode);
        }
        
        bool WebSocket::has_data_message()
        {
                return (frame_header_.opcode == kTextOpcode
                        || frame_header_.opcode == kBinaryOpcode
                        || frame_header_.opcode == kContinutationOpcode);
        }

        bool WebSocket::has_complete_data_message()
        {
                return has_data_message()
                        && is_data_message_complete();
        }

        bool WebSocket::is_data_message_complete()
        {
                return has_unfragmented_text_message()
                        || has_unfragmented_binary_message()
                        || has_final_continuation_message();
        }
        
        bool WebSocket::has_unfragmented_text_message()
        {
                return (frame_header_.opcode == kTextOpcode && frame_header_.fin);
        }

        bool WebSocket::has_unfragmented_binary_message()
        {
                return (frame_header_.opcode == kBinaryOpcode && frame_header_.fin);
        }

        bool WebSocket::has_final_continuation_message()
        {
                return (frame_header_.opcode == kContinutationOpcode && frame_header_.fin);
        }

        void WebSocket::handle_control_message()
        {
                switch (frame_header_.opcode) {
                case kCloseOpcode:
                        answer_close_handshake();
                        break;
                
                case kPingOpcode:
                        send_pong();
                        break;
                
                case kPongOpcode:
                        handle_pong_response();
                        break;
                default:
                        log_error("WebSocket::handle_control_message: Invalid opcode ");
                        break;
                }
        }

        void WebSocket::answer_close_handshake()
        {
                get_close_code();
                closing_send_reply();
        }

        void WebSocket::get_close_code()
        {
                remote_close_reason_ = kCloseMissingCode;
                if (input_payload_buffer_.size() == 2) {
                        remote_close_reason_ =
                                (CloseCode) convert_to_uint16(input_payload_buffer_);
                }
        }

        void WebSocket::closing_send_reply()
        {
                send_close_message(remote_close_reason_);                
                close_connection();
        }

        bool WebSocket::send_close_message(CloseCode code)
        {
                uint8_t data[2];
                uint16_t netshort;
                rcom::MemBuffer payload;
                
                netshort = htons((uint16_t)code);
                data[0] = (uint8_t) (netshort & 0x00ff);
                data[1] = (uint8_t) ((netshort & 0xff00) >> 8);
                payload.append(data, 2);
                
                rcom::MemBuffer message;
                make_message(message, kCloseOpcode, payload);
                
                return socket_send(message);
        }

        void WebSocket::close_without_handshake()
        {
                close_connection();
        }

        void WebSocket::close_with_handshake(CloseCode code)
        {
                try {
                        try_close_with_handshake(code);
                } catch (...) {
                        log_error("close_with_handshake failed");
                        close_without_handshake();
                }
        }
        
        void WebSocket::try_close_with_handshake(CloseCode code)
        {
                if (send_close_message(code)) {
                        closing_wait_reply(5.0);
                } else {
                        log_warning("WebSocket::close_with_handshake: failed to send close. "
                               "Not waiting for a reply");
                }
                
                close_connection();
        }
        
        void WebSocket::closing_wait_reply(double timeout)
        {
                double start_time = rcom_time(get_linux());
                double remaining_time = timeout;

                while (remaining_time >= 0.0) {
                        
                        WaitStatus status = socket_wait(0.010);
                        if (status == kWaitOK) {
                                
                                read_message();
                                if (frame_header_.opcode == kCloseOpcode) {
                                        break;
                                }
                        }

                        remaining_time = compute_remaning_time(start_time, timeout);
                }
        }
        
        void WebSocket::send_pong()
        {
                rcom::MemBuffer message;
                make_message(message, kPongOpcode, input_payload_buffer_);                
                socket_send(message); // FIXME: what if send fails? RecvError?...
        }

        void WebSocket::handle_pong_response()
        {
                log_info("WebSocket: Got pong response"); // TODO
        }

        void WebSocket::handle_data_payload(rcom::MemBuffer& message)
        {
                assert_continuation();
                assert_message_length(message);
                set_payload_type();
                copy_payload(message);
                prepare_continuation();
        }

        void WebSocket::assert_continuation()
        {
                bool valid = ((frame_header_.opcode == kContinutationOpcode
                               && is_continuation_)
                              || (frame_header_.opcode != kContinutationOpcode
                                  && !is_continuation_));
                if (!valid) {
                        log_error("WebSocket: Unexpected continuation");
                        throw RecvError("Unexpected continuation", kCloseProtocolError);
                }
        }

        void WebSocket::assert_message_length(rcom::MemBuffer& message)
        {
                size_t length = input_payload_buffer_.size();
        
                if (frame_header_.opcode == kContinutationOpcode)
                        length += message.size();
        
                if (length > kMaximumMessageLength) {
                        log_error("WebSocket: Message too big (%lu B)", length);
                        throw RecvError("Message too big", kCloseTooBig);
                }
        }

        void WebSocket::set_payload_type()
        {
                if (frame_header_.opcode == kTextOpcode) {                
                        is_text_ = true;
                } else if (frame_header_.opcode == kBinaryOpcode) {                
                        is_text_ = false;
                }
        }

        void WebSocket::copy_payload(rcom::MemBuffer& message)
        {
                if (frame_header_.opcode == kTextOpcode
                    || frame_header_.opcode == kBinaryOpcode) {
                        message.clear();
                }
                message.append(input_payload_buffer_);
        }

        void WebSocket::prepare_continuation()
        {
                is_continuation_ = (frame_header_.fin == 0);
        }

        
        bool WebSocket::send(rcom::MemBuffer& message, MessageType type)
        {
                bool success = false;
                size_t length = message.size();
                
                if (length >= kMaximumMessageLength) {
                        log_error("WebSocket::send: Message too long (max. %d MB)",
                              (int) (kMaximumMessageLength / (1024 * 1024)));
                } else if (length < kShortMessageLength) {
                        success = send_short_data_message(message, type);
                } else {
                        success = send_long_data_message(message, type);
                }
                
                return success;
        }
        
        bool WebSocket::send_short_data_message(rcom::MemBuffer& message, MessageType type)
        {
                Opcode opcode = (type == kTextMessage)? kTextOpcode : kBinaryOpcode;
                make_message(opcode, message);
                turn_buffering_off();
                return send_output_buffer();
        }
        
        bool WebSocket::send_long_data_message(rcom::MemBuffer& message, MessageType type)
        {
                Opcode opcode = (type == kTextMessage)? kTextOpcode : kBinaryOpcode;

                turn_buffering_on();

                return send_long_message_header(opcode, message)
                        && send_long_message_payload(message);
        }

        bool WebSocket::send_long_message_header(Opcode opcode, rcom::MemBuffer& message)
        {
                output_message_buffer_.clear();
                output_append_header(output_message_buffer_, opcode, message);
                return send_output_buffer();
        }

        // Send the payload in blocks of 1k
        bool WebSocket::send_long_message_payload(rcom::MemBuffer& message)
        {
                bool success = true;
                const std::vector<uint8_t>& data = message.data();
                size_t length = message.size();
                size_t sent = 0;
                size_t n;
                
                while (success && sent < length) {
                
                        n = kPayloadBufferLength;
                        if (sent + n > length)
                                n = length - sent;

                        success = output_send_payload(&data[sent], n);
                        sent += n;
                }
                
                return success;
        }        
        
        void WebSocket::make_message(Opcode opcode, rcom::MemBuffer& message)
        {
                make_message(output_message_buffer_, opcode, message);
        }
        
        void WebSocket::make_message(rcom::MemBuffer& output,
                                     Opcode opcode,
                                     rcom::MemBuffer& message)
        {
                output.clear();
                output_append_header(output, opcode, message);
                output_append_payload(output, message);
        }
        
        void WebSocket::apply_mask(uint8_t *out, const uint8_t *in,
                                   size_t length, uint8_t *mask)
        {
                for (uint64_t i = 0, j = 0; i < length; i++) {
                        out[i] = in[i] ^ mask[j++];
                        if (j == 4) j = 0;
                }
        }

        void WebSocket::turn_buffering_off()
        {
                socket_->turn_buffering_off();
        }
        
        void WebSocket::turn_buffering_on()
        {
                socket_->turn_buffering_on();
        }

        bool WebSocket::send_output_buffer()
        {
                return socket_send(output_message_buffer_);
        }
        
        bool WebSocket::socket_send(rcom::MemBuffer& buffer)
        {
                bool success = true;
                
                if (!socket_->send(buffer)) {
                        log_error("WebSocket::socket_send: write failed. closing");
 
                        /* If writing the TCP socket fails, don't bother going
                         * through the closing handshake (the code may
                         * loop...). Just close the socket. */
                        close_without_handshake();                
                        success = false;
                }
                return success;
        }
        
        bool WebSocket::socket_send(const uint8_t *buffer, size_t length)
        {
                bool success = true;
                
                if (!socket_->send(buffer, length)) {
                        log_error("WebSocket::socket_send: write failed. closing");
 
                        /* If writing the TCP socket fails, don't bother going
                         * through the closing handshake (the code may
                         * loop...). Just close the socket. */
                        close_without_handshake();                
                        success = false;
                }
                return success;
        }

        void WebSocket::make_accept_string(std::string& accept, std::string& key)
        {
                unsigned char digest[21];
                memset(digest, 0, sizeof(digest));
                
                std::string data;
                data = key;
                data += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
                
                SHA1(data, digest);
                encode_base64(digest, 20, accept);
        }
}
