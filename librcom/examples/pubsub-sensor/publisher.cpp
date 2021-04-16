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
#include <signal.h>
#include <stdlib.h>
#include <r.h>
#include <MessageHub.h>
#include <IMessageListener.h>

static bool quit = false;
void quit_on_control_c();

using namespace rcom;
using namespace rpp;

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

void broadcast_sensor_value(MessageHub& hub)
{
        double temperature = get_sensor_value();
        MemBuffer message;
        message.printf("The temperature is %.1f °C", temperature);
        hub.broadcast(message);
}

int main()
{
        try {
                MessageHub hub("sensor");                
                
                quit_on_control_c();
        
                while (!quit) {
                        
                        /* Don't forget to handle the incoming client
                         * connections. */
                        hub.handle_events();
                        
                        broadcast_sensor_value(hub);
                        
                        clock_sleep(1.0);
                }
                
        } catch (std::runtime_error& re) {
                r_err("main: caught runtime_error: %s", re.what());
        } catch (...) {
                r_err("main: caught exception");
        }
}

void set_quit(int sig, siginfo_t *info, void *ucontext)
{
        (void) sig;
        (void) info;
        (void) ucontext;
        quit = true;
}

void quit_on_control_c()
{
        struct sigaction act;
        memset(&act, 0, sizeof(struct sigaction));

        act.sa_flags = SA_SIGINFO;
        act.sa_sigaction = set_quit;
        if (sigaction(SIGINT, &act, nullptr) != 0) {
                perror("init_signal_handler");
                exit(1);
        }
}
