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
#include "rcom/ip.h"

using namespace std;

namespace rcom {

        Address::Address() : Address(0)
        {
        }

        Address::Address(uint16_t port) : Address("0.0.0.0", port)
        {
                std::string ip = get_local_ip();
                set(ip.c_str(), port);
        }
        
        Address::Address(const char *ip, uint16_t port) : addr_{AF_INET,0,0,{0}}
        {
                std::string ipaddress;
                if (ip == nullptr)
                        ipaddress = get_local_ip();
                else
                        ipaddress = ip;
                set(ipaddress.c_str(), port);
        }
 
        Address::Address(const std::string& str) : addr_{AF_INET,0,0,{0}}
        {
                parse(str);
        }

        Address::Address(IAddress& address) : addr_{AF_INET,0,0,{0}}
        {
                addr_ = address.get_sockaddr();
        }

        void Address::set(const IAddress& other)
        {
                if (&other != this) {
                        addr_ = other.get_sockaddr();
                }
        }

        string& Address::ip(string& s)
        {
                s = inet_ntoa(addr_.sin_addr);
                return s;
        }
        
        uint16_t Address::port()
        {
                return ntohs(addr_.sin_port);
        }
        
        void Address::set(const char *ip, uint16_t port)
        {
                addr_.sin_family = AF_INET;
                set_ip(ip);
                set_port(port);
        }
        
        void Address::set(const std::string& str)
        {
                parse(str);
        }

        void Address::set_ip(const char *ip)
        {
                if (ip != nullptr) {
                        if (inet_aton(ip, &addr_.sin_addr) == 0) {
                                throw std::runtime_error("Address::set_ip: "
                                                         "inet_aton failed");
                        }
                } else {
                        throw std::runtime_error("Address::set_ip: null address");
                }
        }
        
        void Address::set_port(uint16_t port)
        {
                addr_.sin_port = htons(port);
        }
        
        void Address::parse(const std::string& address)
        {
                string ip;
                string portstr;
                
                string::const_iterator split_colon = std::find(address.rbegin(),
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

        bool Address::is_valid_integer(string& s)
        {
                bool success = false;
                try {
                        std::stoi(s);
                        success = true;                        
                } catch (...) {
                }
                return success;
        }

        bool Address::is_set()
        {
                return (addr_.sin_port != 0)
                        && (addr_.sin_addr.s_addr != 0);
        }
                
        string& Address::tostring(string& str)
        {
                ip(str);
                str += ":";
                str += std::to_string(port());
                return str;
        }

        struct sockaddr_in Address::get_sockaddr() const 
        {
                return addr_;
        }

}
