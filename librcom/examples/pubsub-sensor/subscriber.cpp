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
#include <syslog.h>
#include <atomic>

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

static void print_sensor_value(rcom::MessageLink& link)
{
        rpp::MemBuffer message;
        if (link.recv(message, 2.0)) {
                std::cout << message.tostring() << std::endl;
        } else {
                r_err("main: receive failed");
        }
}

int main()
{
        std::signal(SIGINT, SignalHandler);

        try {
                rcom::MessageLink link("sensor");
                
                while (!quit) {
                        print_sensor_value(link);
                }                
                
        } catch (std::runtime_error& re) {
                r_err("main: caught runtime_error: %s", re.what());
        } catch (...) {
                r_err("main: caught exception");
        }
}

