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
#include "ConsoleLogger.h"
#include "HttpParser.h"

namespace rcom {

        const double parse_timeout_seconds = 5.0;

        HttpParser::HttpParser()
                : state_(HttpParser::kErrorState),
                  status_code_(),
                  status_message_(),
                  buffer_(),
                  name_()
        {}
        
        bool HttpParser::parse_request(ISocket& socket)
        {
                state_ = kRequestMethod;
                return parse(socket);
        }

        bool HttpParser::parse_response(ISocket& socket)
        {
                state_ = kResponseVersion;
                return parse(socket);
        }

        bool HttpParser::parse(ISocket& socket)
        {
                bool success = false;
                buffer_.clear();

                while (true) {
                        WaitStatus status = socket.wait(parse_timeout_seconds);
                        if (status == kWaitOK) {
                                success = read_char(socket);
                                if (!success)
                                        break;
                                if (state_ == kBody)
                                        break;
                        } else {
                                log_error("HttpParser: Failed to read the socket (status=%s)",
                                      status == kWaitTimeout? "timeout" : "error");
                                break;
                        }
                }
                return success;
        }

        bool HttpParser::read_char(ISocket& socket)
        {
                uint8_t c;
                return socket.read(&c, 1) && handle_char(c);
        }

        bool HttpParser::is_valid_version_char(uint8_t c)
        {
                return strchr("HTTP/0123456789.", c) != nullptr;
        }
        
        bool HttpParser::handle_char(uint8_t c)
        {
                switch (state_) {
                case kRequestMethod:
                        if (buffer_.size() > 8) {
                                error(kHttpStatusBadRequest, "Method string too long");
                        } else if (c == ' ') {
                                if (set_method())
                                        state_ = kRequestMethodSpaces;
                        } else if (isupper(c)) {
                                buffer_.put(c);
                        } else {
                                error(kHttpStatusBadRequest, "Unexpected char in method");
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
                                error(kHttpStatusURITooLong, "URI too long");
                        } else if (c == ' ') {
                                if (set_uri())
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
                                if (set_version())
                                        state_ = kRequestLineLF;
                        } else if (buffer_.size() > 8) {
                                error(kHttpStatusBadRequest, "HTTP version too long");
                        } else if (!is_valid_version_char(c)) {
                                error(kHttpStatusBadRequest,
                                      "Invalid character in HTTP version");
                        } else {
                                buffer_.put(c);
                        } 
                        break;
                
                case kRequestLineLF:
                        if (c == '\n') {
                                state_ = kHeaderStart;
                        } else {
                                error(kHttpStatusBadRequest,
                                      "Expected CRLF while reading request");
                        }
                        break;

                case kResponseVersion:
                        if (buffer_.size() > 8) {
                                error(kHttpStatusBadRequest, "HTTP version too long");
                        } else if (c == ' ') {
                                if (set_version())
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
                                error(kHttpStatusBadRequest, "HTTP code too long");
                        } else if (c == ' ') {
                                if (set_code())
                                        state_ = kResponseCodeSpaces;
                        } else if (isdigit(c)) {
                                buffer_.put(c);
                        } else {
                                error(kHttpStatusBadRequest,
                                      "Unexpected char in status code");
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
                                error(kHttpStatusEntityTooLarge,
                                      "HTTP status reason too long");
                        } else if (c == '\r') {
                                if (set_reason())
                                        state_ = kResponseReasonLF;
                        } else {
                                buffer_.put(c);
                        } 
                        break;
                
                case kResponseReasonLF:
                        if (c == '\n') {
                                state_ = kHeaderStart;
                        } else {
                                error(kHttpStatusBadRequest,
                                      "Expected CRLF while reading status");
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
                                error(kHttpStatusBadRequest,
                                      "Unexpected char in header name");
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
                                        error(kHttpStatusEntityTooLarge,
                                              "Header name too long");
                                }
                        } else {
                                error(kHttpStatusBadRequest,
                                      "Unexpected char in header name");
                        }
                        break;

                case kHeaderSpaces:
                        if (c == ' ') {
                                // skip
                        } else if (c == '\r') {
                                error(kHttpStatusBadRequest,
                                      "Unexpected CR header value");
                        } else {
                                buffer_.clear();
                                buffer_.put(c);
                                state_ = kHeaderValue;
                        }
                        break;
                
                case kHeaderValue:
                        if (c == '\r') {
                                if (add_header())
                                        state_ = kHeaderLF;
                        } else if (buffer_.size() < kMaxHeaderValueLength) {
                                buffer_.put(c);
                        } else {
                                error(kHttpStatusEntityTooLarge, "Header value too long");
                        }
                        break;

                case kHeaderLF:
                        if (c == '\n') {
                                state_ = kHeaderStart;
                        } else {
                                error(kHttpStatusBadRequest,
                                      "Expected CRLF at end of header");
                        }
                        break;
                
                case kHeadersEndLF:
                        if (c == '\n') {
                                state_ = kBody;
                        } else {
                                error(kHttpStatusBadRequest,
                                      "Expected CRLF at end of headers");
                        }
                        break;

                case kBody:
                case kErrorState:
                default:
                        log_warning("http_parser_handle_char: reached invalid state");
                        break;
                }
        
                return state_ == kErrorState? false : true;
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

        void HttpParser::error(int code, const char *what)
        {
                log_warning("HttpParser: %s", what);
                status_code_ = code;
                status_message_ = what;
                state_ = kErrorState;
        }

        void HttpParser::store_header_name()
        {
                name_ = buffer_.tostring();
        }
        
}
