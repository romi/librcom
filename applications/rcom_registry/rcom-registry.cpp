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
#include <rcom/Registry.h>
#include <rcom/RegistryServer.h>
#include <rcom/WebSocketServer.h>
#include <rcom/ServerSocket.h>
#include <rcom/Address.h>
#include <rcom/util.h>
#include <rcom/RegistryLookupServer.h>
#include <rcom/RegistryLookup.h>

std::atomic<bool> quit(false);

void SignalHandler(int signal)
{
        if (signal == SIGSEGV) {
                syslog(1, "rcom-registry segmentation fault");
                exit(signal);
        } else if (signal == SIGINT) {
                std::cout << "Ctrl-C Quitting Application" << std::endl;
                quit = true;
        } else {
                std::cout << "Unknown signal received: " << signal << std::endl;
        }
}

int main(int argc, const char **argv)
{
        int result = 0;
        rcom::ConsoleLog log;
        rcom::Linux system(log);

        std::signal(SIGSEGV, SignalHandler);
        std::signal(SIGINT, SignalHandler);
        
        try {
                std::string ip = system.local_ip();
                if (argc == 2) {
                        ip = argv[1];
                }
                
                rcom::Address address(ip, 10101);
                rcom::Registry registry;
                
                std::shared_ptr<rcom::IWebSocketFactory> factory
                        = std::make_shared<rcom::WebSocketFactory>(log, system);

                std::unique_ptr<rcom::IServerSocket> server_socket
                        = std::make_unique<rcom::ServerSocket>(log, system, address);
                
                rcom::RegistryServer registry_server(registry, log);
                
                rcom::WebSocketServer server(server_socket, factory, registry_server, log);

                std::string s = address.tostring();
                log_info(log, "Registry server running at %s", s.c_str());

                rcom::RegistryLookupServer lookup_server(log, system, s, rcom::kLookupPort);
                
                while (!quit) {
                        server.handle_events();
                        system.sleep(0.010);
                }

                log_debug(log, "Quitting");
                
        } catch (nlohmann::json::exception& je) {
                std::cout << "main: caught JSON error: " << je.what() << std::endl;
                result = 1;
        } catch (std::runtime_error& re) {
                std::cout << "main: caught runtime_error: " << re.what() << std::endl;
                result = 2;
        } catch (...) {
                std::cout << "main: caught exception" << std::endl;
                result = 3;
        }

        rcom::log_debug(log, "End");

        return result;
}

