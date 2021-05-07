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

#include <Linux.h>
#include <SocketFactory.h>
#include <Registry.h>
#include <RegistryServer.h>
#include <WebSocketServer.h>
#include <ServerSocket.h>
#include <Address.h>
#include <ClockAccessor.h>

std::atomic<bool> quit(false);

void SignalHandler(int signal)
{
        if (signal == SIGSEGV){
                syslog(1, "rcom-registry segmentation fault");
                exit(signal);
        }
        else if (signal == SIGINT){
                r_info("Ctrl-C Quitting Application");
                perror("init_signal_handler");
                quit = true;
        }
        else{
                r_err("Unknown signam received %d", signal);
        }
}

int main()
{
        try {
                // FIXME
                r_log_init();
                r_log_set_app("rcom-registry");
        
                std::shared_ptr<rcom::ISocketFactory> factory
                        = std::make_shared<rcom::SocketFactory>();
                rcom::Registry registry;
                rcom::Address address(10101);
                std::unique_ptr<rpp::ILinux> linux = std::make_unique<rpp::Linux>();
                std::unique_ptr<rcom::IServerSocket> server_socket
                        = std::make_unique<rcom::ServerSocket>(linux, address);
                std::shared_ptr<rcom::IMessageListener> registry_server
                        = std::make_shared<rcom::RegistryServer>(registry);
                rcom::WebSocketServer server(server_socket, factory, registry_server);

                std::signal(SIGSEGV, SignalHandler);
                std::signal(SIGINT, SignalHandler);

                std::string s;
                address.tostring(s);
                r_info("Registry server running at %s.", s.c_str());
                
                while (!quit) {
                        server.handle_events();
                        rpp::ClockAccessor::GetInstance()->sleep(0.050);
                }
                
        } catch (JSONError& je) {
                r_err("main: caught JSON error: %s", je.what());
        } catch (std::runtime_error& re) {
                r_err("main: caught runtime_error: %s", re.what());
        } catch (...) {
                r_err("main: caught exception");
        }
}

