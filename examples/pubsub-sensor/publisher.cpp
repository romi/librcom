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
#include <syslog.h>
#include <atomic>
#include <unistd.h>

#include <rcom/Linux.h>
#include <rcom/MessageHub.h>
#include <rcom/IMessageListener.h>
#include <rcom/util.h>

std::atomic<bool> quit(false);

void SignalHandler(int signal)
{
        if (signal == SIGSEGV){
                syslog(1, "rcom-registry segmentation fault");
                exit(signal);
        } else if (signal == SIGINT){
                std::cout << "Ctrl-C Quitting Application" << std::endl;
                perror("init_signal_handler");
                quit = true;
        } else {
                std::cout << "Unknown signal, received " << signal << std::endl;
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

void broadcast_sensor_value(rcom::IMessageHub& hub)
{
        double temperature = get_sensor_value();
        rcom::MemBuffer message;
        message.printf("The temperature is %.1f °C", temperature);
        hub.broadcast(message, rcom::kTextMessage, nullptr);
}

int main()
{
        try {
                auto hub = rcom::MessageHub::create("sensor");
                
                std::signal(SIGINT, SignalHandler);

                while (!quit) {
                        
                        /* Don't forget to handle the incoming client
                         * connections. */
                        hub->handle_events();
                        
                        broadcast_sensor_value(*hub);
                        
                        sleep(1);
                }
                
        } catch (std::runtime_error& re) {
                std::cout << "main: caught runtime_error: " << re.what() << std::endl;
        } catch (...) {
                std::cout << "main: caught exception" << std::endl;
        }
}

