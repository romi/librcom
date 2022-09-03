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
#ifndef _LIBRCOM_I_HTTP_PARSER_LISTENER_H_
#define _LIBRCOM_I_HTTP_PARSER_LISTENER_H_

#include <string>

namespace rcom {

        class HttpParser;
        
        class IHttpParserListener
        {
        public:
                virtual ~IHttpParserListener() = default;
                
        protected:
                
                friend class HttpParser;
                
                virtual void set_method(std::string& value) = 0;
                virtual void set_uri(std::string& value) = 0;
                virtual void set_version(std::string& value) = 0;
                virtual void set_code(std::string& value) = 0;
                virtual void set_reason(std::string& value) = 0;
                virtual void set_header(std::string& name, std::string& value) = 0;
        };
}

#endif // _LIBRCOM_I_HTTP_PARSER_LISTENER_H_

