#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <string>
#include <iostream>
#include "rcom/ConsoleLogger.h"

#define ONE_KB_BUFFER (1024)
#define LOG_INFO_SIZE (48) // Time, Type, Name, thread_id


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
