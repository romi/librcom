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
#ifndef _LIBRCOM_ADDRESS_H_
#define _LIBRCOM_ADDRESS_H_

#include <string>
#include "IAddress.h"

namespace rcom {

        class Address : public IAddress
        {
        protected:
                struct sockaddr_in addr_;

                bool set_ip(const char *ip);
                bool set_port(uint16_t port);
                bool is_valid_integer(std::string& s);
                bool parse(const std::string& str);

        public:
                Address();
                Address(uint16_t port);
                Address(const char *ip, uint16_t port);
                Address(const std::string& str);
                Address(IAddress& address);

                virtual ~Address() = default;
                
                bool set(const char *ip, uint16_t port) override;
                bool set(const std::string& str) override;
                bool set(const IAddress& other) override;
                bool is_set() override;
                std::string& tostring(std::string& str) override;
                
                struct sockaddr_in get_sockaddr() const override;

                std::string& ip(std::string&);
                uint16_t port();
        };
}

#endif // _LIBRCOM_ADDRESS_H_

