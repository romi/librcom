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
#include <cstring>
#include "rcom/IAddress.h"

namespace rcom {

        class Address : public IAddress
        {
        protected:
                struct sockaddr_in addr_;

                void set_ip(const char *ip);
                void set_port(uint16_t port);
                bool is_valid_integer(std::string& s);
                void parse(const std::string& str);

        public:
                Address();
                explicit Address(uint16_t port);
                explicit Address(const char *ip, uint16_t port);
                explicit Address(const std::string& str);
                explicit Address(IAddress& address);
                ~Address() override = default;


                bool operator==(const Address &rval) const {
                    auto other_addr = rval.get_sockaddr();
                    return (std::memcmp(&addr_, (void*)&other_addr, sizeof(addr_)) == 0);
                }
                
                void set(const char *ip, uint16_t port) override;
                void set(const std::string& str) override;
                void set(const IAddress& other) override;
                bool is_set() override;
                std::string& tostring(std::string& str) override;
                
                struct sockaddr_in get_sockaddr() const override;

                std::string& ip(std::string&);
                uint16_t port();
        };
}

#endif // _LIBRCOM_ADDRESS_H_

