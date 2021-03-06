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
#include "ConsoleLogger.h"
#include <MessageLink.h>

int main()
{
        try {
                rcom::MessageLink link("hello-world");
                rcom::MemBuffer message;
                message.append_string("hello");

                if (link.send(message)
                    && link.recv(message, 1.0)) {

                        std::cout << "Server replies '" << message.tostring() << "'"
                                  << std::endl;
                        
                } else {
                        log_error("main: failed to send & receive the message");
                }
                
                
        } catch (std::runtime_error& re) {
                log_error("main: caught runtime_error: %s", re.what());
        } catch (...) {
                log_error("main: caught exception");
        }
}
