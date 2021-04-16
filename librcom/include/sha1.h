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

/*
    SHA-1 in C
    By Steve Reid <steve@edmweb.com>
    100% Public Domain
*/
#ifndef _LIBRCOM_SHA1_H_
#define _LIBRCOM_SHA1_H_

#include <stdint.h>

namespace rcom {

        struct SHA1_CTX {
                uint32_t state[5];
                uint32_t count[2];
                unsigned char buffer[64];
        };

        void SHA1Transform(uint32_t state[5], const unsigned char buffer[64]);
        void SHA1Init(SHA1_CTX* context);
        void SHA1Update(SHA1_CTX* context, const unsigned char* data, size_t len);
        void SHA1Final(unsigned char digest[20], SHA1_CTX* context);
}

#endif // _LIBRCOM_SHA1_H_
