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

#include <rcom/Linux.h>
#include <rcom/SocketFactory.h>
#include <rcom/Registry.h>
#include <rcom/RegistryServer.h>
#include <rcom/WebSocketServer.h>
#include <rcom/ServerSocket.h>
#include <rcom/Address.h>
#include <rcom/ConsoleLog.h>
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
        try {
                std::signal(SIGSEGV, SignalHandler);
                std::signal(SIGINT, SignalHandler);
                
                // FIXME
                const char *ip = nullptr;
                if (argc == 2)
                        ip = argv[1];
                
                rcom::Address address(ip, 10101);
                rcom::Registry registry;
                
                std::shared_ptr<rcom::ILinux> linux
                        = std::make_shared<rcom::Linux>();
                
                std::shared_ptr<rcom::ILog> log
                        = std::make_shared<rcom::ConsoleLog>();

                std::shared_ptr<rcom::ISocketFactory> factory
                        = std::make_shared<rcom::SocketFactory>(linux, log);

                std::unique_ptr<rcom::IServerSocket> server_socket
                        = std::make_unique<rcom::ServerSocket>(linux, log, address);
                
                std::shared_ptr<rcom::IMessageListener> registry_server
                        = std::make_shared<rcom::RegistryServer>(registry, log);
                
                rcom::WebSocketServer server(server_socket, factory, registry_server, log);

                std::string s;
                address.tostring(s);
                std::cout << "Registry server running at " << s.c_str() << std::endl;

                rcom::RegistryLookupServer lookup_server(s, rcom::kLookupPort);
                
                while (!quit) {
                        server.handle_events();
                        usleep(20000);
                }
                
        } catch (nlohmann::json::exception& je) {
                std::cout << "main: caught JSON error: " << je.what() << std::endl;
        } catch (std::runtime_error& re) {
                std::cout << "main: caught runtime_error: " << re.what() << std::endl;
        } catch (...) {
                std::cout << "main: caught exception" << std::endl;
        }
}

