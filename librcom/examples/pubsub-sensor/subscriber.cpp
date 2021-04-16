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
#include <memory>
#include <signal.h>
#include <r.h>
#include <MessageLink.h>

using namespace rcom;
using namespace rpp;

static void quit_on_control_c();
static bool quit = false;

static void print_sensor_value(MessageLink& link)
{
        MemBuffer message;
        if (link.recv(message, 2.0)) {
                std::cout << message.tostring() << std::endl;
        } else {
                r_err("main: receive failed");
        }
}

int main()
{
        quit_on_control_c();
        
        try {
                MessageLink link("sensor");                
                
                while (!quit) {
                        print_sensor_value(link);
                }                
                
        } catch (std::runtime_error& re) {
                r_err("main: caught runtime_error: %s", re.what());
        } catch (...) {
                r_err("main: caught exception");
        }
}

static void set_quit(int sig, siginfo_t *info, void *ucontext)
{
        (void) sig;
        (void) info;
        (void) ucontext;
        quit = true;
}

static void quit_on_control_c()
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
