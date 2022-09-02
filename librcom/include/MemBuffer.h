/*
  ROMI libr

  Copyright (C) 2019 Sony Computer Science Laboratories
  Author(s) Peter Hanappe

  The libr library provides some hardware abstractions and low-level
  utility functions.

  libr is free software: you can redistribute it and/or modify it
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
#ifndef R_MEMBUFFER_H
#define R_MEMBUFFER_H

#include <stdint.h>
#include <string>
#include <vector>

namespace rcom
{
        class MemBuffer
        {
        protected:
                static void vprintf(std::string& instring, const char* format, va_list ap);

        public:
                MemBuffer() : data_() {}
                MemBuffer(const MemBuffer& buffer);
                MemBuffer& operator=(const MemBuffer& from);
                bool operator==(const MemBuffer& rval);
                virtual ~MemBuffer() = default;
        
                void put(uint8_t c);
                void append(const uint8_t *data, size_t len);
                void append(const MemBuffer& buffer);
                void append_string(const char *string);
                void printf(const char* format, ...);
        
                [[nodiscard]] const std::vector<uint8_t>& data() const;
                std::string tostring();
                size_t size();        
                void clear();

        private:
                std::vector<uint8_t> data_;
        };
}

#endif // R_MEMBUFFER_H
