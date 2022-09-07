/*
  ROMI librpp

  Copyright (C) 20120 Sony Computer Science Laboratories
  Author(s) Peter Hanappe, Douglas Boari

  The librpp library provides some hardware abstractions and low-level
  utility functions.

  librpp is free software: you can redistribute it and/or modify it
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

#include <cstring>
#include <stdexcept>
#include <stdarg.h>

#include "rcom/MemBuffer.h"
#include "rcom/Log.h"

namespace rcom
{
        MemBuffer::MemBuffer(const MemBuffer& buffer) : data_()
        {
                append(buffer);
        }

        MemBuffer &MemBuffer::operator=(const MemBuffer &from) {
                    clear();
                    append(from);
                    return *this;
        }

        bool MemBuffer::operator==(const MemBuffer &rval) {
            return (data_ == rval.data());
        }

        void MemBuffer::put(uint8_t c)
        {
                data_.emplace_back(c);
        }

        void MemBuffer::append(const uint8_t *data, size_t len)
        {
                data_.insert(data_.end(), data, data+len);
        }

        void MemBuffer::append(const MemBuffer& buffer)
        {
                data_.insert(data_.end(), buffer.data().begin(), buffer.data().end());
        }

        void MemBuffer::append(const std::string& s)
        {
                data_.insert(data_.end(), s.data(), s.data() + s.length());
        }

        void MemBuffer::append_string_32k_max(const char *string)
        {
                const int KB_32 = (32 * 1024);
                size_t lens = strnlen(string, KB_32 + 1);
                if (lens == KB_32 + 1)
                        throw std::runtime_error("MemBuffer::append_string_32k_max() "
                                                 "string too long");
                data_.insert(data_.end(), (uint8_t *) string, (uint8_t *) string+lens);
        }

        void MemBuffer::printf(const char *format, ...)
        {
                va_list ap;
                std::string formatted_string;
                va_start(ap, format);
                vprintf(formatted_string, format, ap);
                va_end(ap);
                append(formatted_string);
        }
        
        void MemBuffer::vprintf(std::string& instring, const char* format, va_list ap)
        {
                va_list ap_copy;
                va_copy(ap_copy, ap);
                size_t size = (size_t) std::vsnprintf(nullptr, 0, format, ap);
                std::vector<char> output(++size, '\0');
                std::vsnprintf(&output[0], size, format, ap_copy);
                va_end(ap_copy);
                instring = output.data();
        }

        const std::vector<uint8_t>& MemBuffer::data() const
        {
                return data_;
        }

        std::string MemBuffer::tostring()
        {
                std::string data_string((const char *) data_.data(), size());
                return data_string;
        }

        void MemBuffer::clear()
        {
                data_.clear();
        }

        size_t MemBuffer::size()
        {
                return data_.size();
        }
}
