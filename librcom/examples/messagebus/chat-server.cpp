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
#include <r.h>
#include <MessageHub.h>
#include <IMessageListener.h>

static bool quit = false;
static void set_quit(int sig, siginfo_t *info, void *ucontext);
static void quit_on_control_c();

using namespace std;
using namespace rcom;
using namespace rpp;

class ChatBus : public IMessageListener
{
public:
        MessageHub *hub_;
        
        ChatBus() : hub_(nullptr) {} 
        ~ChatBus() = default; 

        ChatBus(ChatBus& b) = delete;
        ChatBus& operator=(const ChatBus& other) = delete;

        void onmessage(IWebSocket& websocket,
                       rpp::MemBuffer& message,
                       rcom::MessageType type) override {
                (void) type; // Tell the compiler it's not used
                cout << "> " << message.tostring() << endl;
                /* Broadcast the incoming message to all connected
                 * clients but exclude the sender. */
                hub_->broadcast(message, &websocket);
        }
};


int main()
{
        try {
                ChatBus chat;
                MessageHub chat_hub("chat", chat);                
                chat.hub_ = &chat_hub;
                
                quit_on_control_c();
        
                while (!quit) {
                        chat_hub.handle_events();
                        clock_sleep(0.050);
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
