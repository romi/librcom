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
#ifndef _LIBRCOM_ILOG_H
#define _LIBRCOM_ILOG_H

namespace rcom {

        class ILog
        {
        public:
                virtual ~ILog() = default;
                virtual void error(const char *format, ...) = 0;
                virtual void warn(const char *format, ...) = 0;
                virtual void info(const char *format, ...) = 0;
                virtual void debug(const char *format, ...) = 0;
        };
}

#endif // _LIBRCOM_ILOG_H
