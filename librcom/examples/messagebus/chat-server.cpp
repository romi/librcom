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
#include <ClockAccessor.h>
#include <log.h>
#include <MessageHub.h>
#include <IMessageListener.h>
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

class ChatBus : public rcom::IMessageListener
{
public:
        rcom::MessageHub *hub_;
        
        ChatBus() : hub_(nullptr) {} 
        ~ChatBus() = default; 

        ChatBus(ChatBus& b) = delete;
        ChatBus& operator=(const ChatBus& other) = delete;

        void onmessage(rcom::IWebSocket& websocket,
                       rpp::MemBuffer& message,
                       rcom::MessageType type) override {
                (void) type; // Tell the compiler it's not used
                std::cout << "> " << message.tostring() << std::endl;
                /* Broadcast the incoming message to all connected
                 * clients but exclude the sender. */
                hub_->broadcast(message, &websocket);
        }
};


int main()
{
        try {
                ChatBus chat;
                rcom::MessageHub chat_hub("chat", chat);
                chat.hub_ = &chat_hub;
                auto clock = rpp::ClockAccessor::GetInstance();

                std::signal(SIGINT, SignalHandler);
        
                while (!quit) {
                        chat_hub.handle_events();
                        clock->sleep(0.050);
                }
                
        } catch (std::runtime_error& re) {
                r_err("main: caught runtime_error: %s", re.what());
        } catch (...) {
                r_err("main: caught exception");
        }
}

