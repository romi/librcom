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
#include <r.h>
#include <MessageLink.h>

static int quit = 0;
void quit_on_control_c();

using namespace std;
using namespace rcom;
using namespace rpp;


/* The rcom library is not thread-safe. So we use the mutex to
 * synchronize the access to the message link. */
mutex mutex_;

void print_available_messages(MessageLink *link)
{
        lock_guard<mutex> lock(mutex_);
        MemBuffer message;
        
        /* The loop will treat all the available messages. When there
         * are no more messages (and after waiting for an additional
         * 10 ms) the loop will quit.  */
        while (link->recv(message, 0.010)) {
                cout << "> " << message.tostring() << endl;
        }
}

void handle_incoming_messages(MessageLink *link)
{
        while (!quit) {
                print_available_messages(link);
                clock_sleep(0.100);
        }
}

void readline(MemBuffer& message)
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

void send_message(MessageLink *link, MemBuffer& message)
{
        lock_guard<mutex> lock(mutex_);
        cout << "< " << message.tostring() << endl;
        link->send(message);
}

void handle_user_messages(MessageLink *link)
{
        MemBuffer message;
        while (!quit) {
                readline(message);
                if (message.size() > 0) {
                        send_message(link, message);
                }
        }
}

int main()
{
        try {
                MessageLink link("chat");                
                MemBuffer message;
                        
                quit_on_control_c();

                /* One thread will handle the incoming messages while
                 * another will read the user input. */
                thread incoming(handle_incoming_messages, &link);
                thread outgoing(handle_user_messages, &link);
                        
                cout << "Type your message in the console and press enter." << endl;
                
                incoming.join();
                outgoing.join();

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
        if (sigaction(SIGINT, &act, NULL) != 0) {
                perror("init_signal_handler");
                exit(1);
        }
}
