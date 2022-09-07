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
#include <string.h>
#include <stdarg.h>
#include <iostream>
#include "rcom/ConsoleLog.h"

namespace rcom {

        ConsoleLog::ConsoleLog()
        {
        }

        void ConsoleLog::error(const std::string& message)
        {
                std::cout << "ERROR: " << message << std::endl;
        }
        
        void ConsoleLog::warn(const std::string& message)
        {
                std::cout << "WARN: " << message << std::endl;
        }
        
        void ConsoleLog::info(const std::string& message)
        {
                std::cout << "INFO: " << message << std::endl;
        }
        
        void ConsoleLog::debug(const std::string& message)
        {
                std::cout << "DEBUG: " << message << std::endl;
        }        
}
