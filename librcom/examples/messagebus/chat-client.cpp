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
#include <thread>
#include <mutex>
#include <syslog.h>
#include <atomic>

#include <Linux.h>
#include <MessageLink.h>
#include <ConsoleLogger.h>
#include <util.h>

std::atomic<bool> quit(false);

void SignalHandler(int signal)
{
        if (signal == SIGSEGV){
                syslog(1, "rcom-registry segmentation fault");
                exit(signal);
        } else if (signal == SIGINT){
                log_info("Ctrl-C Quitting Application");
                perror("init_signal_handler");
                quit = true;
        } else {
                log_error("Unknown signam received %d", signal);
        }
}

/* The rcom library is not thread-safe. So we use the mutex to
 * synchronize the access to the message link. */
std::mutex mutex_;

void print_available_messages(rcom::MessageLink *link)
{
        std::lock_guard<std::mutex> lock(mutex_);
        rcom::MemBuffer message;
        
        /* The loop will treat all the available messages. When there
         * are no more messages (and after waiting for an additional
         * 10 ms) the loop will quit.  */
        while (link->recv(message, 0.010)) {
                std::cout << "> " << message.tostring() << std::endl;
        }
}

void handle_incoming_messages(rcom::MessageLink *link)
{
        rcom::Linux linux;
        while (!quit) {
                print_available_messages(link);
                rcom_sleep(linux, 0.100);
        }
}

void readline(rcom::MemBuffer& message)
{
        message.clear();
        
        while (!quit) {
                int c = getchar();
                if (c == '\n' || c == EOF)
                        break;
                else if (c < 0)
                        break;
                else
                        message.put((uint8_t) c);
        }
}

void send_message(rcom::MessageLink *link, rcom::MemBuffer& message)
{
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "< " << message.tostring() << std::endl;
        link->send(message);
}

void handle_user_messages(rcom::MessageLink *link)
{
        rcom::MemBuffer message;
        while (!quit) {
                readline(message);
                if (message.size() > 0) {
                        send_message(link, message);
                }
        }
}

int main(int argc, char **argv)
{
        const char *topic = "chat";

        if (argc >= 2)
                topic = argv[1];
        
        try {
                rcom::MessageLink link(topic);
                rcom::MemBuffer message;

                std::signal(SIGINT, SignalHandler);

                /* One thread will handle the incoming messages while
                 * another will read the user input. */
                std::thread incoming(handle_incoming_messages, &link);
                std::thread outgoing(handle_user_messages, &link);
                        
                std::cout << "Type your message in the console and press enter." << std::endl;
                
                incoming.join();
                outgoing.join();

        } catch (std::runtime_error& re) {
                log_error("main: caught runtime_error: %s", re.what());
        } catch (...) {
                log_error("main: caught exception");
        }
}

