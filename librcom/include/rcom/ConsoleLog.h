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
#ifndef _LIBRCOM_CONSOLELOG_H
#define _LIBRCOM_CONSOLELOG_H

#include <stdarg.h>
#include <cstring>
#include <memory>
#include <mutex>
#include "rcom/ILog.h"

namespace rcom {

        static const size_t kLogBufferSize = 1024;

        class ConsoleLog : public ILog
        {
        public:
                ConsoleLog();
                ~ConsoleLog() override = default;

                void error(const std::string& message) override;
                void warn(const std::string& message) override;
                void info(const std::string& message) override;
                void debug(const std::string& message) override;
        };
}


#endif // _LIBRCOM_CONSOLELOG_H
