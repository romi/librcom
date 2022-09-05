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
#include <syslog.h>
#include <atomic>

#include <rcom/MessageLink.h>
#include <rcom/Linux.h>
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

int main()
{
        try {

                rcom::MessageLink link("speed");
                uint8_t data[1024];
                rcom::Linux linux;
                rcom::MemBuffer message;
                message.append(data, 1024);

                uint64_t total_bytes = 0;
                double start_time = rcom_time(linux);
                double next_time = start_time + 1.0;

                std::signal(SIGINT, SignalHandler);
                
                while (!quit) {
                        
                        if (link.send(message)
                            && link.recv(message, 1.0)) {

                                total_bytes += message.size();

                                double now = rcom_time(linux);
                                if (now > next_time) {
                                        next_time += 1.0;
                                        printf("Bandwidth: %.3f MB/s\n",
                                               (double) total_bytes / (now - start_time) / 1048576.0);
                                }
                        
                        } else {
                                std::cout << "main: failed to send & receive the message"
                                          << std::endl;
                                break;
                        }
                }
                
        } catch (std::runtime_error& re) {
                std::cout << "main: caught runtime_error: " << re.what() << std::endl;
        } catch (...) {
                std::cout << "main: caught exception" << std::endl;
        }
}
