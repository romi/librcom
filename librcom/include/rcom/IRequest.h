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
#ifndef _LIBRCOM_I_REQUEST_H_
#define _LIBRCOM_I_REQUEST_H_

#include <string>

namespace rcom {

        class IRequest
        {
        public:
                enum Method {
                        kGetMethod,
                        kUnsupportedMethod
                };
                        
                virtual ~IRequest() = default; 

                virtual Method get_method() = 0;
                virtual void set_method(Method method) = 0;
                
                virtual std::string& get_uri() = 0;
                virtual void set_uri(const std::string& uri) = 0;

                virtual void add_header(const std::string& name,
                                        const std::string& value) = 0;
                virtual bool get_header_value(const std::string& name,
                                              std::string& value) = 0;
                
                virtual void assert_websocket() = 0;
        };
}

#endif // _LIBRCOM_I_REQUEST_H_
