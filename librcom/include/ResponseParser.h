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
#ifndef _LIBRCOM_RESPONSE_PARSER_H_
#define _LIBRCOM_RESPONSE_PARSER_H_

#include "IResponseParser.h"
#include "IHttpParserListener.h"
#include "HttpParser.h"

namespace rcom {

        class ResponseParser : public IResponseParser, public HttpParser
        {
        protected:
                IResponse& response_;

        public:
                ResponseParser(IResponse& response);                
                virtual ~ResponseParser() = default; 

                bool parse(ISocket& socket) override;
                IResponse& response() override;

        protected:
                                
                bool set_method() override;
                bool set_uri() override;
                bool set_version() override;
                bool set_code() override;
                bool set_reason() override;
                bool add_header() override;
        };
}

#endif // _LIBRCOM_RESPONSE_PARSER_H_

