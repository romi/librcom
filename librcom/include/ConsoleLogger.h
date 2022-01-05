#ifndef ROMI_ROVER_BUILD_AND_TEST_CONSOLELOGGER_H
#define ROMI_ROVER_BUILD_AND_TEST_CONSOLELOGGER_H

#include <cstring>

void log_error(const char* format, ...);
void log_warning(const char* format, ...);
void log_debug(const char* format, ...);
void log_info(const char* format, ...);

#endif //ROMI_ROVER_BUILD_AND_TEST_CONSOLELOGGER_H
