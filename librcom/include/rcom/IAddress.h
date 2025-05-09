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
#ifndef _LIBRCOM_IADDRESS_H_
#define _LIBRCOM_IADDRESS_H_

#include <string>

namespace rcom {

        class IAddress
        {
        public:
                virtual ~IAddress() = default;

                virtual void set(const std::string& ip, uint16_t port) = 0;
                virtual void set(const std::string& str) = 0;
                virtual void set(const IAddress& other) = 0;
                virtual bool is_set() const = 0;

                virtual const std::string& ip() const = 0;
                virtual uint16_t port() const = 0;
                virtual std::string tostring() const = 0;
        };
}

#endif // _LIBRCOM_IADDRESS_H_

