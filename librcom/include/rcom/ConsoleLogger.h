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
#ifndef _LIBRCOM_CONSOLELOGGER_H
#define _LIBRCOM_CONSOLELOGGER_H

#include <stdarg.h>
#include <cstring>
#include <mutex>
#include "rcom/ILog.h"

namespace rcom {

        static const size_t kLogBufferSize = 1024;

        class ConsoleLogger : public ILog
        {
        protected:
                char buffer_[kLogBufferSize];
                std::mutex mutex_;

                
                void buffer_printf(const char* format, va_list ap);

        public:
                ConsoleLogger();
                ~ConsoleLogger() override = default;
                void error(const char *format, ...) override;
                void warn(const char *format, ...) override;
                void info(const char *format, ...) override;
                void debug(const char *format, ...) override;
        };

        void log_error(const char* format, ...);
        void log_warning(const char* format, ...);
        void log_debug(const char* format, ...);
        void log_info(const char* format, ...);
}


#endif // _LIBRCOM_CONSOLELOGGER_H
