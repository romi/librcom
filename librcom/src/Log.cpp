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
#include <stdarg.h>
#include <mutex>
#include <iostream>
#include "rcom/Log.h"

namespace rcom {

        static const size_t kLogBufferSize = 1024;
        static char log_buffer_[kLogBufferSize];
        static std::mutex buffer_mutex_;

        using SynchronizedCodeBlock = std::lock_guard<std::mutex>;

        static void buffer_printf(std::string& out, const char* format, va_list ap)
        {
                SynchronizedCodeBlock sync(buffer_mutex_);
                
                memset(log_buffer_, '\0', kLogBufferSize);
                vsnprintf(log_buffer_, kLogBufferSize, format, ap);
                log_buffer_[kLogBufferSize-1] = '\0';
                out = log_buffer_;
        }

        void log_err(const std::shared_ptr<ILog>& log, const char* format, ...)
        {
                std::string message;
                va_list ap;
                va_start(ap, format);
                buffer_printf(message, format, ap);
                va_end(ap);
                log->error(message);
        }

        void log_warn(const std::shared_ptr<ILog>& log, const char* format, ...)
        {
                std::string message;
                va_list ap;
                va_start(ap, format);
                buffer_printf(message, format, ap);
                va_end(ap);
                log->warn(message);
        }

        void log_debug(const std::shared_ptr<ILog>& log, const char* format, ...)
        {
                std::string message;
                va_list ap;
                va_start(ap, format);
                buffer_printf(message, format, ap);
                va_end(ap);
                log->debug(message);
        }

        void log_info(const std::shared_ptr<ILog>& log, const char* format, ...)
        {
                std::string message;
                va_list ap;
                va_start(ap, format);
                buffer_printf(message, format, ap);
                va_end(ap);
                log->info(message);
        }
}
