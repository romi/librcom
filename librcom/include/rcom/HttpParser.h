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
#ifndef _LIBRCOM_HTTP_PARSER_H_
#define _LIBRCOM_HTTP_PARSER_H_

#include "rcom/MemBuffer.h"
#include "rcom/IHttpParserListener.h"
#include "rcom/ISocket.h"
#include "rcom/HttpConstants.h"

namespace rcom {

        class HttpParser
        {
        public:
                const static size_t kMaxHeaderNameLength = 128;
                const static size_t kMaxHeaderValueLength = 1024;
                const static size_t kMaxUriLength = 2048;
                
        protected:
                
                enum State {
                        kRequestMethod,
                        kRequestMethodSpaces,
                        kRequestUri,
                        kRequestUriSpaces,
                        kRequestVersion,
                        kRequestLineLF,
                        
                        kResponseVersion,
                        kResponseVersionSpaces,
                        kResponseCode,
                        kResponseCodeSpaces,
                        kResponseReason,
                        kResponseReasonLF,
                        
                        kHeaderStart,
                        kHeaderName,
                        kHeaderSpaces,
                        kHeaderValue,
                        kHeaderLF,
                        
                        kHeadersEndLF,
                        kBody,
                        kErrorState,
                };
                
                State state_;
                int status_code_;
                std::string status_message_;
                rcom::MemBuffer buffer_;
                std::string name_;
                
                bool parse(ISocket& socket);
                bool read_char(ISocket& socket);
                bool handle_char(uint8_t c);
                bool is_separator(uint8_t c);
                bool is_token_character(uint8_t c);
                void store_header_name();
                void error(int code, const char *what);
                bool is_valid_version_char(uint8_t c);
                
                virtual bool set_method() = 0;
                virtual bool set_uri() = 0;
                virtual bool set_version() = 0;
                virtual bool set_code() = 0;
                virtual bool set_reason() = 0;
                virtual bool add_header() = 0;

        public:
                HttpParser();
                virtual ~HttpParser() = default; 

                bool parse_request(ISocket& socket);
                bool parse_response(ISocket& socket);
                
        };
}

#endif // _LIBRCOM_HTTP_PARSER_H_

