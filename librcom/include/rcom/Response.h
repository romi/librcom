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
#ifndef _LIBRCOM_RESPONSE_H_
#define _LIBRCOM_RESPONSE_H_

#include <vector>
#include "rcom/IResponse.h"

namespace rcom {

        struct ResponseHeader
        {
                std::string name_;
                std::string value_;

                ResponseHeader(const std::string& name, const std::string& value)
                        : name_(name), value_(value)
                        {}
        };

        class Response : public IResponse
        {
        protected:
                int code_;
                std::vector<ResponseHeader> headers_;
                                
                bool header_equals(const std::string& name,
                                   const std::string& expected);
                void status_equals_101();
                void connection_header_equals_upgrade();
                void upgrade_header_equals_websocket();
                void accept_header_is_valid(const std::string& accept);
                
        public:
                Response();
                virtual ~Response() = default; 

                int get_code() override;
                void set_code(int code) override;
                void add_header(const std::string& name,
                                const std::string& value) override;
                bool get_header_value(const std::string& name, std::string& value);
                void assert_websocket(const std::string& accept) override;
        };
}

#endif // _LIBRCOM_RESPONSE_H_

