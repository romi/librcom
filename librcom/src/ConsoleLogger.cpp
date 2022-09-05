#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <string>
#include <iostream>
#include "rcom/ConsoleLogger.h"

#define ONE_KB_BUFFER (1024)

namespace rcom {

        using SynchronizedCodeBlock = std::lock_guard<std::mutex>;

        ConsoleLogger::ConsoleLogger()
                : mutex_()
        {
        }

        void ConsoleLogger::error(const char *format, ...)
        {
                SynchronizedCodeBlock sync(mutex_);
                va_list ap;
                va_start(ap, format);
                buffer_printf(format, ap);
                va_end(ap);
                std::cout << "ERROR: " << buffer_ << std::endl;
        }
        
        void ConsoleLogger::warn(const char *format, ...)
        {
                SynchronizedCodeBlock sync(mutex_);
                va_list ap;
                va_start(ap, format);
                buffer_printf(format, ap);
                va_end(ap);
                std::cout << "WARN: " << buffer_ << std::endl;
        }
        
        void ConsoleLogger::info(const char *format, ...)
        {
                SynchronizedCodeBlock sync(mutex_);
                va_list ap;
                va_start(ap, format);
                buffer_printf(format, ap);
                va_end(ap);
                std::cout << "INFO: " << buffer_ << std::endl;
        }
        
        void ConsoleLogger::debug(const char *format, ...)
        {
                SynchronizedCodeBlock sync(mutex_);
                va_list ap;
                va_start(ap, format);
                buffer_printf(format, ap);
                va_end(ap);
                std::cout << "DEBUG: " << buffer_ << std::endl;
        }

        void ConsoleLogger::buffer_printf(const char* format, va_list ap)
        {
                memset(buffer_, '\0', kLogBufferSize);
                vsnprintf(buffer_, kLogBufferSize, format, ap);
                buffer_[kLogBufferSize-1] = '\0';
        }
        
        
        static char log_buffer[ONE_KB_BUFFER];

        static int log_printf(char* buffer, size_t bufflen, const char* format, va_list ap)
        {
                memset(buffer, 0, bufflen);
                vsnprintf(buffer, bufflen, format, ap);
                return 0;
        }

        void log_error(const char* format, ...)
        {
                va_list ap;
                va_start(ap, format);
                log_printf(log_buffer, ONE_KB_BUFFER, format, ap);
                va_end(ap);
                std::cout << "ERROR: " << std::string(log_buffer) << std::endl;
        }

        void log_warning(const char* format, ...)
        {
                va_list ap;
                va_start(ap, format);
                log_printf(log_buffer, ONE_KB_BUFFER, format, ap);
                va_end(ap);
                std::cout << "WARNING: " << std::string(log_buffer) << std::endl;
        }

        void log_debug(const char* format, ...)
        {
                va_list ap;
                va_start(ap, format);
                log_printf(log_buffer, ONE_KB_BUFFER, format, ap);
                va_end(ap);
                std::cout << "DEBUG: " << std::string(log_buffer) << std::endl;
        }

        void log_info(const char* format, ...)
        {
                va_list ap;
                va_start(ap, format);
                log_printf(log_buffer, ONE_KB_BUFFER, format, ap);
                va_end(ap);
                std::cout << "INFO: " << std::string(log_buffer) << std::endl;
        }
}
