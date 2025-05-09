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
#include "rcom/ISystem.h"
#include "rcom/IAddress.h"

namespace rcom {

        class Address : public IAddress
        {
        protected:
                std::string ip_;
                uint16_t port_;

                void set_ip(const std::string& ip);
                void set_port(uint16_t port);
                bool is_valid_integer(std::string& s);
                void parse(const std::string& str);

        public:
                Address();
                explicit Address(const std::string& ip, uint16_t port);
                explicit Address(const std::string& str); // ip:port
                explicit Address(IAddress& address);
                ~Address() override = default;

                bool operator==(const Address &other) const {
                    auto this_addr = tostring();
                    auto other_addr = other.tostring();
                    return this_addr == other_addr;
                }
                
                void set(const std::string& ip, uint16_t port) override;
                void set(const std::string& str) override;
                void set(const IAddress& other) override;
                bool is_set() const override;
                const std::string& ip() const override;
                uint16_t port() const override;
                std::string tostring() const override;
        };
}

#endif // _LIBRCOM_ADDRESS_H_

