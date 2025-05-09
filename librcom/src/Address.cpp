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
#include <arpa/inet.h>
#include <stdexcept>
#include <algorithm>
#include "rcom/Address.h"

namespace rcom {

        Address::Address() : Address("0.0.0.0", 0)
        {
        }
        
        Address::Address(const std::string& ip, uint16_t port)
                : ip_(ip), port_(0)
        {
                set(ip, port);
        }
 
        Address::Address(const std::string& str)
                : ip_("0.0.0.0"), port_(0)
        {
                parse(str);
        }

        Address::Address(IAddress& address) 
                : ip_(address.ip()), port_(address.port())
        {
        }

        void Address::set(const IAddress& other)
        {
                if (&other != this) {
                        set_ip(other.ip());
                        set_port(other.port());
                }
        }

        const std::string& Address::ip() const
        {
                return ip_;
        }
        
        uint16_t Address::port() const
        {
                return port_;
        }
        
        void Address::set(const std::string& ip, uint16_t port)
        {
                set_ip(ip);
                set_port(port);
        }
        
        void Address::set(const std::string& str)
        {
                parse(str);
        }

        void Address::set_ip(const std::string& ip)
        {
                ip_ = ip;
        }
        
        void Address::set_port(uint16_t port)
        {
                port_ = port;
        }
        
        void Address::parse(const std::string& address)
        {
                std::string ip;
                std::string portstr;
                
                std::string::const_iterator split_colon = std::find(address.rbegin(),
                                                                    address.rend(), ':').base();
                
                if (split_colon != address.begin()) {
                        ip.assign(address.begin(), split_colon-1);
                        portstr.assign(split_colon, address.end());

                        if (is_valid_integer(portstr))
                                set(ip.c_str(), (uint16_t) std::stoi(portstr));
                        else 
                                throw std::runtime_error("Address::parse: invalid port");
                        
                } else {
                        throw std::runtime_error("Address::parse: invalid address");
                }
        }

        bool Address::is_valid_integer(std::string& s)
        {
                bool success = false;
                try {
                        std::stoi(s);
                        success = true;                        
                } catch (...) {
                }
                return success;
        }

        bool Address::is_set() const
        {
                return (ip_ != "0.0.0.0") && (port_ != 0);
        }
                
        std::string Address::tostring() const
        {
                std::string s = ip_;
                s += ":";
                s += std::to_string(port());
                return s;
        }
}
