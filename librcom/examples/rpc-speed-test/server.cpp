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
#include <log.h>
#include <MessageHub.h>
#include <IMessageListener.h>
#include <ClockAccessor.h>
#include <WebSocketServerFactory.h>
#include "ConsoleLogger.h"

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
                log_info("Ctrl-C Quitting Application");
                perror("init_signal_handler");
                quit = true;
        }
        else{
                log_error("Unknown signam received %d", signal);
        }
}

class MessageListener : public rcom::IMessageListener
{
public:
        ~MessageListener() override = default;

        void onmessage(rcom::IWebSocket& websocket,
                       rcom::MemBuffer& message,
                       rcom::MessageType type) override {
                (void) type; // Tell the compiler it's not used
                websocket.send(message, rcom::kTextMessage);
        }
};

int main()
{
        try {
                auto webserver_socket_factory = rcom::WebSocketServerFactory::create();
                std::shared_ptr<rcom::ISocketFactory> socket_factory = std::make_shared<rcom::SocketFactory>();
                std::shared_ptr<rcom::IMessageListener> listener
                        = std::make_shared<MessageListener>();
                rcom::MessageHub message_hub("speed", listener, socket_factory, webserver_socket_factory);
                auto clock = rpp::ClockAccessor::GetInstance();

                std::signal(SIGINT, SignalHandler);
        
                while (!quit) {
                        message_hub.handle_events();
                        clock->sleep(0.001); // FIXME
                }
                
        } catch (std::runtime_error& re) {
                log_error("main: caught runtime_error: %s", re.what());
        } catch (...) {
                log_error("main: caught exception");
        }
}

