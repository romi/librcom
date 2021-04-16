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
#ifndef _LIBRCOM_UTIL_H_
#define _LIBRCOM_UTIL_H_

#include <string>

namespace rcom {

        void SHA1(const std::string& data, unsigned char *digest);
        void encode_base64(const unsigned char *data, size_t length, std::string& out);

        void create_uuid(std::string& s);
        bool is_valid_uuid(const char *s);
        bool is_valid_name(const std::string& name);
        bool is_valid_topic(const std::string& topic);
        bool is_base64_string(const std::string& string);
}

#endif // _LIBRCOM_UTIL_H_

