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
#include <WebSocketServerFactory.h>
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

class ChatBus : public rcom::IMessageListener
{
public:
        rcom::MessageHub *hub_;
        
        ChatBus() : hub_(nullptr) {} 
        ~ChatBus() override = default;

        ChatBus(ChatBus& b) = delete;
        ChatBus& operator=(const ChatBus& other) = delete;

        void onmessage(rcom::IWebSocket& websocket,
                       rpp::MemBuffer& message,
                       rcom::MessageType type) override {
                (void) type; // Tell the compiler it's not used
                std::cout << "> " << message.tostring() << std::endl;
                /* Broadcast the incoming message to all connected
                 * clients but exclude the sender. */
            hub_->broadcast(message, rcom::kTextMessage, &websocket);
        }
};


int main(int argc, char **argv)
{
        const char *topic = "chat";

        if (argc >= 2)
                topic = argv[1];
        
        try {
                auto webserver_socket_factory = rcom::WebSocketServerFactory::create();
                std::shared_ptr<rcom::ISocketFactory> socket_factory = std::make_shared<rcom::SocketFactory>();
                std::shared_ptr<ChatBus> chat = std::make_shared<ChatBus>();
                std::shared_ptr<rcom::IMessageListener> listener = chat;
                rcom::MessageHub chat_hub(topic, listener, socket_factory, webserver_socket_factory);
                chat->hub_ = &chat_hub;
                auto clock = rpp::ClockAccessor::GetInstance();

                std::signal(SIGINT, SignalHandler);
        
                while (!quit) {
                        chat_hub.handle_events();
                        clock->sleep(0.050);
                }
                
        } catch (std::runtime_error& re) {
                log_error("main: caught runtime_error: %s", re.what());
        } catch (...) {
                log_error("main: caught exception");
        }
}

