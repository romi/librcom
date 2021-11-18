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
#include <syslog.h>
#include <atomic>
#include <WebSocketServerFactory.h>

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
                r_err("Unknown signal received %d", signal);
        }
}

class HelloWorldListener : public rcom::IMessageListener
{
public:
        ~HelloWorldListener() override = default;

        void onmessage(rcom::IWebSocket& websocket,
                       rpp::MemBuffer& message,
                       rcom::MessageType type) override {
                (void) type;
                std::cout << "Client says '" << message.tostring() << "'" << std::endl;
                rpp::MemBuffer reply;
                reply.append_string("world");
                websocket.send(reply, rcom::kTextMessage);
        }
};


int main()
{
        try {
                auto webserver_socket_factory = rcom::WebSocketServerFactory::create();
                std::shared_ptr<rcom::IMessageListener> hello_world
                        = std::make_shared<HelloWorldListener>();
                rcom::MessageHub message_hub("hello-world", hello_world, webserver_socket_factory);
                auto clock = rpp::ClockAccessor::GetInstance();

                std::signal(SIGINT, SignalHandler);
        
                while (!quit) {
                        message_hub.handle_events();
                        clock->sleep(0.050);
                }
                
        } catch (std::runtime_error& re) {
                r_err("main: caught runtime_error: %s", re.what());
        } catch (...) {
                r_err("main: caught exception");
        }
}

