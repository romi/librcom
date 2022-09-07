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
#include <string.h>
#include <ctype.h>
#include "rcom/Log.h"
#include "rcom/HttpParser.h"

namespace rcom {

        const double parse_timeout_seconds = 5.0;

        HttpParser::HttpParser()
                : state_(HttpParser::kErrorState),
                  buffer_(),
                  name_()
        {}
        
        void HttpParser::parse_request(ISocket& socket)
        {
                state_ = kRequestMethod;
                parse(socket);
        }

        void HttpParser::parse_response(ISocket& socket)
        {
                state_ = kResponseVersion;
                parse(socket);
        }

        void HttpParser::parse(ISocket& socket)
        {
                buffer_.clear();

                while (true) {
                        WaitStatus status = socket.wait(parse_timeout_seconds);
                        if (status != kWaitOK) {
                                error("Socket timed out");
                        }
                        
                        read_char(socket);
                        
                        if (state_ == kBody)
                                break;
                }
        }

        void HttpParser::read_char(ISocket& socket)
        {
                uint8_t c;
                if (!socket.read(&c, 1)) {
                        error("read_char failed");
                }
                handle_char(c);
        }

        bool HttpParser::is_valid_version_char(uint8_t c)
        {
                return strchr("HTTP/0123456789.", c) != nullptr;
        }
        
        void HttpParser::handle_char(uint8_t c)
        {
                switch (state_) {
                case kRequestMethod:
                        if (buffer_.size() > 8) {
                                error("Method string too long");
                        } else if (c == ' ') {
                                set_method();
                                state_ = kRequestMethodSpaces;
                        } else if (isupper(c)) {
                                buffer_.put(c);
                        } else {
                                error("Unexpected char in method");
                        } 
                        break;

                case kRequestMethodSpaces:
                        if (c == ' ') {
                                // skip
                        } else {
                                buffer_.clear();
                                buffer_.put(c);
                                state_ = kRequestUri;
                        }
                        break;

                case kRequestUri:
                        if (buffer_.size() > kMaxUriLength) {
                                error("URI too long");
                        } else if (c == ' ') {
                                set_uri();
                                state_ = kRequestUriSpaces;
                        } else {
                                buffer_.put(c);
                        } 
                        break;

                case kRequestUriSpaces:
                        if (c == ' ') {
                                // skip
                        } else {
                                buffer_.clear();
                                buffer_.put(c);
                                state_ = kRequestVersion;
                        } 
                        break;
                
                case kRequestVersion:
                        if (c == '\r') {
                                set_version();
                                state_ = kRequestLineLF;
                        } else if (buffer_.size() > 8) {
                                error("HTTP version too long");
                        } else if (!is_valid_version_char(c)) {
                                error("Invalid character in HTTP version");
                        } else {
                                buffer_.put(c);
                        } 
                        break;
                
                case kRequestLineLF:
                        if (c == '\n') {
                                state_ = kHeaderStart;
                        } else {
                                error("Expected CRLF while reading request");
                        }
                        break;

                case kResponseVersion:
                        if (buffer_.size() > 8) {
                                error("HTTP version too long");
                        } else if (c == ' ') {
                                set_version();
                                state_ = kResponseVersionSpaces;
                        } else {
                                buffer_.put(c);
                        } 
                        break;
                
                case kResponseVersionSpaces:
                        if (c == ' ') {
                                // skip
                        } else {
                                buffer_.clear();
                                buffer_.put(c);
                                state_ = kResponseCode;
                        } 
                        break;
                
                case kResponseCode:
                        if (buffer_.size() > 3) {
                                error("HTTP code too long");
                        } else if (c == ' ') {
                                set_code();
                                state_ = kResponseCodeSpaces;
                        } else if (isdigit(c)) {
                                buffer_.put(c);
                        } else {
                                error("Unexpected char in status code");
                        } 
                        break;
                
                case kResponseCodeSpaces:
                        if (c == ' ') {
                                // skip
                        } else {
                                buffer_.clear();
                                buffer_.put(c);
                                state_ = kResponseReason;
                        } 
                        break;
                
                case kResponseReason:
                        if (buffer_.size() > 128) {
                                error("HTTP status reason too long");
                        } else if (c == '\r') {
                                set_reason();
                                state_ = kResponseReasonLF;
                        } else {
                                buffer_.put(c);
                        } 
                        break;
                
                case kResponseReasonLF:
                        if (c == '\n') {
                                state_ = kHeaderStart;
                        } else {
                                error("Expected CRLF while reading status");
                        }
                        break;
                                
                case kHeaderStart:
                        if (c == '\r') {
                                state_ = kHeadersEndLF;
                        } else if (is_token_character(c)) {
                                buffer_.clear();
                                buffer_.put(c);
                                state_ = kHeaderName;
                        } else {
                                error("Unexpected char in header name");
                        }
                        break;

                case kHeaderName:
                        if (c == ':') {
                                store_header_name();
                                state_ = kHeaderSpaces;
                        } else if (is_token_character(c)) {
                                if (buffer_.size() < kMaxHeaderNameLength) {
                                        buffer_.put(c);
                                } else {
                                        error("Header name too long");
                                }
                        } else {
                                error("Unexpected char in header name");
                        }
                        break;

                case kHeaderSpaces:
                        if (c == ' ') {
                                // skip
                        } else if (c == '\r') {
                                error("Unexpected CR header value");
                        } else {
                                buffer_.clear();
                                buffer_.put(c);
                                state_ = kHeaderValue;
                        }
                        break;
                
                case kHeaderValue:
                        if (c == '\r') {
                                add_header();
                                state_ = kHeaderLF;
                        } else if (buffer_.size() < kMaxHeaderValueLength) {
                                buffer_.put(c);
                        } else {
                                error("Header value too long");
                        }
                        break;

                case kHeaderLF:
                        if (c == '\n') {
                                state_ = kHeaderStart;
                        } else {
                                error("Expected CRLF at end of header");
                        }
                        break;
                
                case kHeadersEndLF:
                        if (c == '\n') {
                                state_ = kBody;
                        } else {
                                error("Expected CRLF at end of headers");
                        }
                        break;

                case kBody:
                case kErrorState:
                default:
                        error("http_parser_handle_char: reached invalid state");
                        break;
                }
        }

        bool HttpParser::is_separator(uint8_t c)
        {
                const char *separator_chars = "()<>@,;:\\\'/[]?={} \t";
                return strchr(separator_chars, (char) c) != nullptr;
        }

        bool HttpParser::is_token_character(uint8_t c)
        {
                bool a = iscntrl((char)c);
                bool b = is_separator(c);
                return (!a && !b);
        }

        void HttpParser::error(const char *what)
        {
                state_ = kErrorState;
                std::string message = "HttpParser: ";
                message += what;
                throw std::runtime_error(message);
        }

        void HttpParser::store_header_name()
        {
                name_ = buffer_.tostring();
        }
        
}
