/*

  Copyright (C) 2019 Sony Computer Science Laboratories
  Author(s) Peter Hanappe

 
  Permission to use, copy, modify, and/or distribute this software for
  any purpose with or without fee is hereby granted.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
  WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
  AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
  DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
  OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
  TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
  PERFORMANCE OF THIS SOFTWARE.

 */
#include <iostream>
#include <csignal>
#include <cstdlib>
#include <log.h>
#include <ClockAccessor.h>
#include <MessageHub.h>
#include <IMessageListener.h>
#include <WebSocketServerFactory.h>

#include <syslog.h>
#include <atomic>
#include "ConsoleLogger.h"

std::atomic<bool> quit(false);

void SignalHandler(int signal)
{
        if (signal == SIGSEGV){
                syslog(1, "rcom-registry segmentation fault");
                exit(signal);
        }
        else if (signal == SIGINT){
                log_info("Ctrl-C Quitting Application");
                perror("init_signal_handler");
                quit = true;
        }
        else{
                log_error("Unknown signam received %d", signal);
        }
}


void initialize_random_generator()
{
        static bool initialized = false;
        if (!initialized) {
                time_t t;
                srand((unsigned) time(&t));
                initialized = true;
        }
}

double get_random_value_between(double min, double max)
{
        double r;
        initialize_random_generator();
        r = (double) rand() / (double) RAND_MAX;
        return min + r * (max - min);
}

double get_sensor_value()
{
        return get_random_value_between(18.0, 22.0);
}

void broadcast_sensor_value(rcom::MessageHub& hub)
{
        double temperature = get_sensor_value();
        rcom::MemBuffer message;
        message.printf("The temperature is %.1f °C", temperature);
        hub.broadcast(message, rcom::kTextMessage, nullptr);
}

int main()
{

        try {

                auto webserver_socket_factory = rcom::WebSocketServerFactory::create();
                std::shared_ptr<rcom::ISocketFactory> socket_factory = std::make_shared<rcom::SocketFactory>();

                rcom::MessageHub hub("sensor", socket_factory, webserver_socket_factory);
                auto clock = rpp::ClockAccessor::GetInstance();
                std::signal(SIGINT, SignalHandler);

                while (!quit) {
                        
                        /* Don't forget to handle the incoming client
                         * connections. */
                        hub.handle_events();
                        
                        broadcast_sensor_value(hub);
                        
                        clock->sleep(1.0);
                }
                
        } catch (std::runtime_error& re) {
                log_error("main: caught runtime_error: %s", re.what());
        } catch (...) {
                log_error("main: caught exception");
        }
}

