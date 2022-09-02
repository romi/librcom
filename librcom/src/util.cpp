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
#include <math.h>
#include <regex>
#include <stdexcept>
#include "ConsoleLogger.h"
#include "util.h"
#include "sha1.h"

namespace rcom {
        
        void SHA1(const std::string& data, unsigned char *digest)
        {
                SHA1_CTX context{};
                SHA1Init(&context);
                SHA1Update(&context, (const unsigned char*) &data[0], data.length());
                SHA1Final(digest, &context);
        }

        void encode_base64(const unsigned char *data, size_t length, std::string& out)
        {
                static const char table[] = {
                        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                        'w', 'x', 'y', 'z', '0', '1', '2', '3',
                        '4', '5', '6', '7', '8', '9', '+', '/'
                };

                unsigned int a, b, c;
                unsigned int p;
                
                out = "";
        
                for (size_t i = 0; i < length; ) {
                        size_t n = length - i;
                        if (n > 3)
                                n = 3;

                        switch (n) {
                        case 3:
                                a = (unsigned int) data[i++];
                                b = (unsigned int) data[i++];
                                c = (unsigned int) data[i++];
                                p = (a << 0x10) + (b << 0x08) + c;
                                out += table[(p >> 18) & 0x3F];
                                out += table[(p >> 12) & 0x3F];
                                out += table[(p >> 6) & 0x3F];
                                out += table[p & 0x3F];
                                break;
                        case 2:
                                a = (unsigned int) data[i++];
                                b = (unsigned int) data[i++];
                                p = (a << 0x10) + (b << 0x08);
                                out += table[(p >> 18) & 0x3F];
                                out += table[(p >> 12) & 0x3F];
                                out += table[(p >> 6) & 0x3F];
                                out += '=';
                                break;
                        case 1:
                                a = (unsigned int) data[i++];
                                p = (a << 0x10);
                                out += table[(p >> 18) & 0x3F];
                                out += table[(p >> 12) & 0x3F];
                                out += '=';
                                out += '=';
                                break;
                        default:
                                // Should not happen. Makes the compiler happy.
                                break;
                        }
                }
        }

        bool is_valid_name(const std::string& name)
        {
                return std::regex_match(name, std::regex("[a-z][a-z-]{2,64}"));
        }

        bool is_valid_topic(const std::string& topic)
        {
                return is_valid_name(topic);
        }

        bool is_base64_string(const std::string& s)
        {
                return std::regex_match(s, std::regex("[a-zA-Z0-9+/]+[=]{0,2}"));
        }

        double rcom_time(ILinux& linux)
        {
                struct timespec spec;
                double result;
                
                linux.clock_gettime(CLOCK_REALTIME, &spec);
                result = (double) spec.tv_sec + (double) spec.tv_nsec / 1.0e9;
                
                return result;
        }
        
        void rcom_sleep(ILinux& linux, double seconds)
        {
                struct timespec spec;
                struct timespec remain;
                
                spec.tv_sec = (time_t) floor(seconds);
                double nsec = seconds - (double) spec.tv_sec;
                spec.tv_nsec = (time_t) floor(nsec * 1.0e9);
                int r = linux.clock_nanosleep(CLOCK_REALTIME, 0, &spec, &remain);
                
                if (r != 0) {
                        if (r == EINTR) {
                                log_error("rcom_sleep: Interrupted");
                                throw std::runtime_error("rcom_sleep: Interrupted");
                        } else {
                                log_error("rcom_sleep: Failed: %d", r);
                                throw std::runtime_error("rcom_sleep: Failed");
                        }
                }
        }
}
