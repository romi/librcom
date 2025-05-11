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
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <syslog.h>
#include <atomic>
#include <memory>
#include <iostream>
#include <unistd.h>

#include <rcom/ConsoleLog.h>
#include <rcom/Linux.h>
#include <rcom/Log.h>
#include <rcom/WebSocketFactory.h>
#include <rcom/RegistryProxy.h>
#include <rcom/RegistryServer.h>
#include <rcom/Address.h>

int main(int argc, const char **argv)
{
        int result = 0;
        rcom::ConsoleLog log;
        rcom::Linux system(log);
        
        try {
                rcom::Address address;
                
                if (argc == 2) {
                        address.set(argv[1], 10101);
                } else {
                        rcom::RegistryServer::get_address(address);                
                }

                rcom::log_info(log, "Using registry %s", address.tostring().c_str());
                
                rcom::WebSocketFactory socket_factory(log, system);
                auto socket = socket_factory.new_client_side_websocket(address);

                rcom::RegistryProxy registry(socket, system, log);

                std::vector<rcom::RegistryEntry> list;
                registry.list(list);

                for (size_t i = 0; i < list.size(); i++) {
                        rcom::RegistryEntry& e = list[i];
                        std::cout << e.topic << ", "
                                  << e.address.tostring() << ", "
                                  << e.type << std::endl;
                }
                
        } catch (nlohmann::json::exception& je) {
                std::cerr << "main: caught JSON error: " << je.what() << std::endl;
                result = 1;
        } catch (std::runtime_error& re) {
                std::cerr << "main: caught runtime_error: " << re.what() << std::endl;
                result = 2;
        } catch (...) {
                std::cerr << "main: caught exception" << std::endl;
                result = 3;
        }

        return result;
}

