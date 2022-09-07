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
                MemBuffer buffer_;
                std::string name_;
                
                void parse(ISocket& socket);
                void read_char(ISocket& socket);
                void handle_char(uint8_t c);
                bool is_separator(uint8_t c);
                bool is_token_character(uint8_t c);
                bool is_valid_version_char(uint8_t c);
                void store_header_name();
                void error(const char *what);
                
                virtual void set_method() = 0;
                virtual void set_uri() = 0;
                virtual void set_version() = 0;
                virtual void set_code() = 0;
                virtual void set_reason() = 0;
                virtual void add_header() = 0;

        public:
                HttpParser();
                virtual ~HttpParser() = default; 

                void parse_request(ISocket& socket);
                void parse_response(ISocket& socket);
                
        };
}

#endif // _LIBRCOM_HTTP_PARSER_H_

