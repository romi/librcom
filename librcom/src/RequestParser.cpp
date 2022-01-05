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
#include "RequestParser.h"
#include "HttpParser.h"

namespace rcom {

        RequestParser::RequestParser(IRequest& request)
                : HttpParser(), request_(request)
        {
        }

        bool RequestParser::parse(ISocket& socket)
        {
                return parse_request(socket);
        }

        bool RequestParser::set_method()
        {
                bool success = false;
                std::string value = buffer_.tostring();
                if (value == "GET") {
                        request_.set_method(IRequest::kGetMethod);
                        success = true;
                } else {
                        error(kHttpStatusMethodNotAllowed,
                                      "Method not allowed");
                }
                return success;
        }
        
        bool RequestParser::set_uri()
        {
                std::string value = buffer_.tostring();
                request_.set_uri(value);
                return true;
        }
        
        bool RequestParser::set_version()
        {
                bool success = false;
                std::string value = buffer_.tostring();
                // This function only checks whether the HTTP version is
                // 1.1. The code is currently not designed for HTTP/2 (and
                // websockets are not designed for HTTP/2 and vice versa). Nor
                // should HTTP/1.0 be used anymore.
                if (value.compare("HTTP/1.1") == 0) {
                        success = true;
                } else {
                        error(kHttpStatusHTTPVersionNotSupported,
                                      "Unsupported HTTP version");
                }
                return success;
        }
        
        bool RequestParser::set_code()
        {
                error(kHttpStatusBadRequest,
                      "Didn't expect a response code");
                return false;
        }
        
        bool RequestParser::set_reason()
        {
                error(kHttpStatusBadRequest,
                      "Didn't expect a response reason");
                return false;
        }
        
        bool RequestParser::add_header()
        {
                std::string value = buffer_.tostring();
                request_.add_header(name_, value);
                return true;
        }

        IRequest& RequestParser::request()
        {
                return request_;
        }
}
