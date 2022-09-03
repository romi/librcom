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
#include <stdlib.h>
#include "rcom/ResponseParser.h"

namespace rcom {

        ResponseParser::ResponseParser(IResponse& response)
                : HttpParser(), response_(response)
        {}

        bool ResponseParser::parse(ISocket& socket)
        {
                return parse_response(socket);
        }

        bool ResponseParser::set_method()
        {
                error(kHttpStatusInternalServerError,
                              "Didn't expect a method");
                return false;
        }
        
        bool ResponseParser::set_uri()
        {
                error(kHttpStatusInternalServerError,
                      "Didn't expect an URI");
                return false;
        }
        
        bool ResponseParser::set_version()
        {
                bool success = false;
                std::string value = buffer_.tostring();
                // This function only checks whether the HTTP version is
                // 1.1. The code is currently not designed for HTTP/2 (and
                // websockets are not designed for HTTP/2 and vice versa). Nor
                // should HTTP/1.0 be used anymore.
                if (value == "HTTP/1.1") {
                        success = true;
                } else {
                        error(kHttpStatusHTTPVersionNotSupported,
                                      "Unsupported HTTP version");
                }
                return success;
        }
        
        bool ResponseParser::set_code()
        {
                bool success = false;
                std::string value = buffer_.tostring();
                char *endptr = nullptr;
                long code = strtol(value.c_str(), &endptr, 10);
                if (endptr != nullptr && *endptr == '\0'
                    && code >= 100 && code < 600) {
                        response_.set_code((int)code);
                        success = true;
                } else {
                        error(kHttpStatusInternalServerError,
                              "Invalid status code");
                }
                return success;
        }
        
        bool ResponseParser::set_reason()
        {
                // The "reason" string is not needed
                return true;
        }

        bool ResponseParser::add_header()
        {
                std::string value = buffer_.tostring();
                response_.add_header(name_, value);
                return true;
        }

        IResponse& ResponseParser::response()
        {
                return response_;
        }
}
