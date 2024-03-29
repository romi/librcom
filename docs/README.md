# rcom

rcom is light-weight C++ libary for inter-node communication. All data
is sent over websockets and rcom provides an implementation of both
server-side and client-side websockets.

rcom offers a low-level API that can be used to build several
communication patterns, such the publisher-subscriber pattern
(pub-sub), or a message bus.

rcom also offers a higher-level API that provides the remote procedure
call pattern (RPC). We will discuss this API in more detail
first. After that we will present the generic API.

# Installation

The installation process follows the classical clone/cmake/make
pattern:

```bash
$ git clone -b ci_dev https://github.com/romi/librcom.git
$ cd librcom/
$ mkdir build
$ cd build
$ cmake ..
$ make
```

Then run the tests to make sure all is well:

```bash
$ ctest -V
```


To check the code coverage run:

```bash
$ make librcom_unit_tests_coverage
$ firefox librcom/librcom_unit_tests_coverage/index.html 
```

# Using rcom for remote procedure calls

We will document how to use rcom through C++ API. However, it is
possible to combine rcom with code writen in Python or Javascript,
among other. We will provide some examples further below.

## Using C++

Suppose that you are writing an application called Madness that
controls a bunch of happy monsters on the local network
(whatever...). You design an interface called `IMonster`, as follows:

```c++
#include <string>
#include <iostream>

class IMonster
{
public:
        virtual ~IMonster() = default;
        virtual void jump_around() = 0;
        virtual void gently_scare_someone(const std::string& person_id) = 0;
        virtual double get_energy_level() = 0;
};
```

All the monsters of your application will derive from this interface,
such as the `HappyMonster` below.


```c++
class HappyMonster : public IMonster
{
protected:
        std::string name_;
        double energy_;
public:
        HappyMonster(const std::string name);
        ~HappyMonster() override = default;
        void jump_around() override;
        void gently_scare_someone(const std::string& person_id) override;
        double get_energy_level() override;
};

HappyMonster::HappyMonster(const std::string name)
        : name_(name), energy_(1.0)
{
}

void HappyMonster::jump_around()
{
        std::cout << "Jump around!" << std::endl;
}

void HappyMonster::gently_scare_someone(const std::string& person_id)
{
        std::cout << "Hey " << person_id
                  << ", don't watch that. Watch this. " 
                  << "This is the happy happy monster show."
                  << std::endl;
}

double HappyMonster::get_energy_level()
{
        return energy_;
}
```

You can now write a small application, create a monster, and have it
do things.

```c++
int main(int argc, char** argv)
{
        HappyMonster monster("Elmo");
        monster.gently_scare_someone("you");
        return 0;
}
```

The full code of this example is split over the following files: [monster_simple.cpp](https://github.com/romi/librcom/blob/ci_dev/examples/tutorial/monster_simple.cpp), [IMonster.h](https://github.com/romi/librcom/blob/ci_dev/examples/tutorial/IMonster.h), and [HappyMonster.h](https://github.com/romi/librcom/blob/ci_dev/examples/tutorial/HappyMonster.h)

## The client-side application

In the next step we will write a monster that lives in a remote
application, either on the same machine but in a different process, or
on a remote machine on the local network. We will write a new type of
monster, called `RemoteMonster`.


```c++
#include "rcom/RemoteStub.h"
#include "rcom/RcomClient.h"

class RemoteMonster : public IMonster, public rcom::RemoteStub
{
public:
        RemoteMonster(std::unique_ptr<rcom::IRPCClient>& client);
        ~RemoteMonster() override = default;
        void jump_around() override;
        void gently_scare_someone(const std::string& person_id) override;
        double get_energy_level() override;
};
```

The new class inherits both from `IMonster` and `RemoteStub`. The
latter is part of rcom. You can also see that the `RemoteMonster`
constructor takes an instance of `IRPCClient` as an argument. This
class represents the connection between the local application and the
remote process. As you can see below, this pointer is passed to the
constructor of `RemoteStub` who will use it to send and receive
messages. Normally, you should not have to add arguments to the
constructor or create additional member variables in the
`RemoteMonster` class because it is just a stub that will forward all
requests to the real implementation that lives in a remote process.

```c++
RemoteMonster::RemoteMonster(std::unique_ptr<rcom::IRPCClient>& client)
        : RemoteStub(client)
{
}
```

We still have to implement the methods of our example class. They are
shown below. 

```c++
void RemoteMonster::jump_around()
{
        bool success = execute_simple_request("jump-around");
        if (!success) {
                std::cout << "jump_around failed" << std::endl;
        }
}

void RemoteMonster::gently_scare_someone(const std::string& person_id)
{
        nlohmann::json params;
        params["person-in"] = person_id;
        
        bool success = execute_with_params("gently-scare-someone", params);
        if (!success) {
                std::cout << "gently_scare_someone failed" << std::endl;
        }
}

double RemoteMonster::get_energy_level()
{
        double energy_level = -1.0;
        nlohmann::json result;

        bool success = execute_with_result("get-energy-level", result);
        if (success) {
                energy_level = result["energy-level"];
        } else {
                std::cout << "get_energy_level failed" << std::endl;
        }
        
        return energy_level;
}
```

The implementation mostly calls upon the methods provided by
`RemoteStub`:

* Use `execute_simple_request` for methods that don't take any
  arguments and return no values.
* Use `execute_with_params` when the caller has to send arguments, but no
  return value is expected.
* Use `execute_with_result` when there are no arguments but a
  value is returned.  
* Finally, the generic method `execute` takes arguments for the remote
  method and returns a value.

Both the parameters and the return value are sent using the
[JSON](https://www.json.org/json-en.html) format. The RemoteStub takes
care of the encoding the data to a JSON string representation and
parsing the incoming string to a C++ JSON data structure. For this
rcom uses the [JSON library](https://github.com/nlohmann/json) by
Niels Lohmann. Check out its documentation to get to know all its
features.

The various execute methods return `true` when the remote method was
executed successfully and `false` when an error occured. They do not
throw an exception. This leaves the choice up to you whether to throw
an exception in response to a failed invokation or not. When an error
occured, the `RemoteStub` will write a message with to the rcom
logger. See more on the log system below.

NOTE: The other functions, such as `RcomClient::create` below do throw
exceptions.

Here is the main function, again, rewriten for the use of the remote
monster:

```c++
int main()
{
        try {
                auto client = rcom::RcomClient::create("elmo", 10.0);
                RemoteMonster monster(client);        
                monster.gently_scare_someone("you");
        } catch (std::exception& e) {
                log_error("main: '%s'", e.what());
        }
        return 0;
}
```

The function `rcom::RcomClient::create` establishes the connection to
a remote object on the local network (or local machine) identified by
"elmo". The second argument is a timeout for the connection. If "elmo"
doesn't show up within 10 seconds, the application calls it quits.

If the connection is established, it is passed to the `RemoteMonster`
object. The application can then call the `IMonster` methods as if the
remote monster was a normal, local object.

The full code of the new version can be found in
[monster_client.cpp](https://github.com/romi/librcom/blob/ci_dev/examples/tutorial/monster_client.cpp).

## The registry

If you run the example application above, it will quit with the
following error message:

```bash
ERROR: Socket::connect: failed to bind the socket
ERROR: Socket::Socket: Failed to connect to address 192.168.1.100:10101
ERROR: main: 'Socket: Failed to connect'
```

In order for the example above to find the "elmo" object, `rcom` uses
another service called the `rcom-registry`. It is basically a
directory service the maps identifiers to IP addresses. You will have
to start the service separately:

```bash
$ ./bin/rcom-registry 
INFO: Registry server running at 192.168.1.100:10101.
```

If you run the example application again, it will still quit. This
time, after 10 seconds, it will show the error message below:

```bash
WARNING: MessageLink::connect: Failed to obtain address for topic 'elmo'
ERROR: MessageLink: Failed to connect: elmo
ERROR: main: 'MessageLink: Failed to connect'
```

This is normal: we didn't implement and start the remote process,
yet. We will look into that in the next session.

## The server-side application

The remote side - or server side - will receive requests coming from
the application that was introduced above. These requests are sent as
JSON strings. They have to be parsed and mapped to the methods of the
actual C++ object that the remote client wants to address. For this,
we will use an adaptor, as follows:

```c++
int main()
{
        try {
                std::string name = "elmo";
                HappyMonster monster(name);
                MonsterAdaptor adaptor(monster);
                auto monster_server = rcom::RcomServer::create(name, adaptor);

                while (true) {
                        monster_server->handle_events();
                        usleep(1000);
                }
                        
        } catch (std::exception& e) {
                log_error("main: '%s'", e.what());
        }
        return 0;
}
```

The `MonsterAdaptor` instance sits in between the generic `RcomServer`
object and the `HappyMonster` object. The server will handle incoming
JSON requests and call the adapter. The adaptor must map the request
to the Monster object. Any return values will be converted to JSON by
the server and sent back.

The key here is the adapter class. It looks as follows:

```c++
class MonsterAdaptor : public rcom::IRPCHandler
{
protected:
        IMonster& monster_;
                
public:
        MonsterAdaptor(IMonster& monster);
        ~MonsterAdaptor() override = default;
        
        void execute(const std::string& method, nlohmann::json& params,
                     nlohmann::json& result, rcom::RPCError& status) override;
        void execute(const std::string& method, nlohmann::json& params,
                     rcom::MemBuffer& result, rcom::RPCError &status) override;
};
```

The two `execute` methods will be called by the server instance. The
first one is for JSON text messages. The second one is for methods
returning large binary data. The use of binary data will be discussed
later.

In our example, the `execute` method checks the value of the `method`
argument and then dispatches the call to the appropriate methods on
the "real" C++ object:

```c++
void MonsterAdaptor::execute(const std::string& method, nlohmann::json& params,
                             nlohmann::json& result, rcom::RPCError& error)
{
        error.code = 0;
        if (method == "jump-around") {
                monster_.jump_around();
                
        } else if (method == "gently-scare-someone") {
                std::string id = params["person-id"];
                monster_.gently_scare_someone(id);
                
        } else if (method == "get-energy-level") {
                result["energy-level"] = monster_.get_energy_level();
                
        } else {
                error.code = rcom::RPCError::kMethodNotFound;
                error.message = "Unknown method";
        }
}
```

That's it! The full code of this section can be found here:
[monster_server.cpp](https://github.com/romi/librcom/blob/ci_dev/examples/tutorial/monster_server.cpp).

## Run the example

To run the example, you must first start the `rcom-registry`:

```bash
$ build/bin/rcom-registry
INFO: Registry server running at 192.168.1.100:10101.
```

Then, in another shell, you start the server-side application that
runs the remote object:


```bash
$ build/bin/monster_server
```

The rcom-registry console should display something like the message
below. It shows that the remote server successfully registered with
the "elmo" identifier.

```
INFO: RegistryServer: Received message: {"request": "register", "topic": "elmo", "address": "192.168.1.100:45175"}
INFO: RegistryServer: Register topic 'elmo' at 192.168.1.100:45175
```

In a third shell, you can now start the client application:

```bash
$ build/bin/monster_client
```

This example application will quit almost immediately because it
doesn't do anything other than send a simple message. The console of
`monster_server` should show the following, though:

```
Hey you, don't watch that. Watch this. This is the happy happy monster show.
```


## Returning binary data

To send binary data in the textual JSON format, it has to be encoded,
for example, using the [Base64](https://en.wikipedia.org/wiki/Base64)
encoding. This can be quite a performance hit. For example, when the
Raspberry Pi Zero has to transmit images, this encoding becomes a
showstopper.

So, it is therfore possible to return the data as a binary
buffer. This is the reason for the second `execute` method in the
adapter class discussed above.


On the client side, you will have to do the following: 

```c++
rcom::MemBuffer& MyClass::call_method_with_binary_output(rcom::MemBuffer& buffer)
{
        nlohmann::json params;
        RPCError error;

        buffer.clear();
        client_->execute("method-id", params, buffer, error);
        
        if (error.code != 0) {
                // ...
        }

        return buffer;
}
```

In the example above, we don't use the `execute` methods of the stub
but directly the `execute` method of the client connection maintained
the stub.

Currently, it is only possible to retrive binary data from the server.
There is no method, yet, for sending a buffer of binary data to the
server. If you have to send binary data, you will have to encode it
and sending it as part of the JSON request.


# The generic API

`rcom` provides both server-side and client-side websockets. We'll call
them client end-point and server end-points. A separate application,
called 'rcom-registry' is a directory server that maintains the list
of all server end-points. The rcom-registry application should be
launched separately before any other application.

The server end-points are identified using a topic, which is a
free-form string. The topic should be unique for a given
rcom-registry. Client end-points that want to communicate with a
server first contact the rcom-registry to obtain the address of the
server end-point. The address is simply a combination of IP address
and port number. The client can then connect to the server end-point
directly.

An application can open several server end-points. And a single server
end-point can handle many clients.

The rcom library does not impose any format on the messages sent back
and forth between the client and the server. Since the websocket
standard makes a distinction between text-based, so does rcom. But
under the hood, rcom is agnostic about the content of the messages.

# The logger

By default, the rcom libray logs the internal messages, including
error messages, to the console. If you are writing a large
application, you probably want to redirect these messages to a file or
a GUI window. In that case, you can subclass the `rcom::ILog` interface
and inject it into the API functions discussed so far. For example, in
the example discussed previously, we created a client connection to a
remote object as follows:


```c++
int main()
{
        // ...
        auto client = rcom::RcomClient::create("elmo", 10.0);
        // ...
}
```

This can be adapted as follows:

```c++
#include "MyLog.h"

int main()
{
        // ...
        auto log = std::make_shared<MyLog>();
        auto client = rcom::RcomClient::create("elmo", 10.0, log);
        // ...
}
```

The class `MyLog` implements the `rcom::Ilog` interface. It must
handle the four types of messages that may be sent by the library as
follows:

```c++
#include <iostream.h>
#include <rcom/ILog.h>
        
class MyLog : public rcom::ILog
{
public:
        MyLog() {}
        ~MyLog() override = default;
                
        void error(const std::string& message) override {
                std::cout << "MyErr: " << message << std::endl;
        }
                
        void warn(const std::string& message) override {
                std::cout << "MyWarn: " << message << std::endl;
        }
                        
        void info(const std::string& message) override {
                std::cout << "MyInfo: " << message << std::endl;
        }
                
        void debug(const std::string& message) override {
                std::cout << "MyDebug: " << message << std::endl;
        }
};
```

Similarly, for the server-side, you can pass your own the `ILog` object:


```c++
#include "MyLog.h"

int main()
{
        // ...
        auto monster_server = rcom::RcomServer::create(name, adaptor, log);
        // ...
}
```


# Fixed port

# No registration

# Security


# Specifying the address of the registry

# Behind a web server 

## http

## https

# Connecting from Javascript

TODO: This section is work in progress (as is most of this documentation BTW).

Connecting to a remote object from Javascript is a two-step process:

1. Create a websocket to rcom-registry to obtain the address of the
requested object.

```javascript
function createRemoteMonster(name, registry)
{
    var registrySocket = new WebSocket('ws://' + registry + ':10101');

    registrySocket.onopen = function (event) {
        var request = { 'request': 'get', 'topic': name };
        registrySocket.send(JSON.stringify(request));
    };

    registrySocket.onmessage = function (event) {
        console.log(event.data);
        var reply = JSON.parse(event.data);
        if (reply.success) {
            registrySocket.close();
            monster = new RemoteMonster(reply.address);
        }
    }
}
```

2. Create a websocket to the remote object using the obtained address.

```javascript
class RemoteMonster
{
    constructor(address) {
        this.socket = new WebSocket('ws://' + address);
        this.socket.onmessage = (event) => {
            this.handleMessage(event.data);
        };
        this.socket.onopen = (event) => {
             // ...
        };
    }

    handleMessage(buffer) {
        var response = JSON.parse(buffer);
        if (response.error) {
            this.handleErrorMessage(response.error);
        } else if (response.method == 'get-energy-level') {
               console.log('RemoteMonster: Energy level ' + response['energy-level']
        }   
    }
    
    handleErrorMessage(err) {
        console.log('RemoteMonster: Method: ' + response.method
                    + ', Error: ' + response.error.message);
    }  

    execute(method, params) {
        var request = { 'method': method, 'params': params };
        var s = JSON.stringify(request);
        this.socket.send(s);
    }  

    jumpAround() {
        this.execute('jump-around');
    }  

    gentlyScareSomeone(id) {
        this.execute('gently-scare-someone', {'person-id': id}};
    }  

    getEnergyLevel() {
        this.execute('get-energy-level');
    }  
}
```


# Connecting from Python

The `rcom` library provides some helper code to exchange data between
Python code and rcom objects written in C++. At the current
development stage, this Python code has only been used for prototyping
during development. The code is not production ready but it may help
to get started in your own projects.

In the root directory of the rcom repository, you will find a
directory called `python` that contains the Python `rcom` modules and
some examples. You can install the rcom Python code and dependencies
as follows:

```bash
$ cd python
$ python3 setup.py install --user
```

## A Python client connecting to an C++ rcom server

To run the example, start the rcom-registry server in a new shell:

```bash
$ bin/rcom-registry
```

In another shell, start the remote monster server that we discussed
above:

```bash
$ bin/monster_server
```

Finally, run the Python client:

```bash
$ python3 examples/monster_client.py
```

The Python code looks as follows. First, we define a new class
`RemoteMonster` that subclasses the `RcomClient` from the
`rcom.rcom_client` module.


```python
from rcom.rcom_client import RcomClient
    
class RemoteMonster(RcomClient):
    
        def __init__(self, name, registry):
            super().__init__(name, registry)

        def jump_around(self):
            self.execute('jump-around')
        
        def gently_scare_someone(self, person_id):
            self.execute('gently-scare-someone', {'person-id': person_id})
        
        def get_energy_level(self):
            answer = self.execute('get-energy-level')
            return answer['energy-level']
```

TODO: The implementation still requires that you pass the IP address
to the registry to the `RcomClient` instance. You can find the local
IP address using this code snippet:

```python
import socket

def get_local_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("8.8.8.8", 80))
    ip = s.getsockname()[0]
    s.close()
    return ip
```

Calling the remote C++ object is now very straightforward:

```python
monster = RemoteMonster('elmo', get_local_ip())
monster.jump_around()
monster.gently_scare_someone('you')
energy = monster.get_energy_level()
print(f'energy level is {energy}')
```



# Overview of the classes and code

`ILinux`, `Linux`, `MockLinux`: To facilitate unit testing, the system
functions are abstracted in the `ILinux` interface. The `Linux` class
provides the default implementation, and `MockLinux` the
implementation used for testing.

The interface `ISocket` defines a standard TCP/IP socket API. The
class `Socket` is the default implementation of the API. Similarly,
`IServerSocket` defines the API for a socket that accepts incoming
connection. It's default implementation can be found in the
`ServerSocket` class. Both `Socket` and `ServerSocket` actually share
a lot of functionality. This functionality is grouped together in the
class `BaseSocket`, which encapsulates the standard BSD socket
interface. Both Both `Socket` and `ServerSocket` delegate most of the
methods to `BaseSocket`.

Websockets have there own API, defined in `IWebSocket`. This interface
basically defines the methods to send or receive a message. The
`WebSocket` class provides the default implementation. It uses an
`ISocket` to send and receive data on the TCP/IP connection and then
implements the websocket protocol as defined in [RFC
6455](https://www.rfc-editor.org/rfc/rfc6455).

Most of the code doesn't create WebSockets directly but uses an
instance of `ISocketFactory` to create them. Again, this facilitates
the testing of the code by passing in a `MockSocketFactory`.

The `WebSocketServer` implements a server that waits for incoming
websocket connections and creates a new `ServerSideWebSocket` after a
successful handshake. It also maintains the list of all open
connections. This allows to send broadcast messages to all client
connected to this server. The `handle_events` method should be called
regularly to deal with the incoming connection requests.

We distinguish between server-side and client-side websockets:

* `ServerSideWebSocket`: The websocket created on the server-side in
response to a new incoming connection.
* `ClientSideWebSocket`: The websocket created by the client to connect
to a `WebSocketServer`.

Both inherit implementation from the `WebSocket` class.

A `MessageHub` is like a `WebSocketServer` with the following
additional functionality:

* It has a topic name.
* It registers the topic and its address to the remote registry.

## RPC classes

IRPCHandler

IRPCClient

IRPCServer

IMessageListener

RcomClient

RemoteStub

RcomServer

RcomMessageHandler

### TODO

Remote access
4G router, set-up at the farm
Managing an fleet of rovers
4G router with a solar panel
queue management
doc format messages
describe format message for different actions: move, path, grab, ...
c++ -> Python
