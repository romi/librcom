master  
![Build Status](https://github.com/romi/librcom/workflows/CI/badge.svg?branch=master)
[![codecov](https://codecov.io/gh/romi/librcom/branch/master/graph/badge.svg)](https://codecov.io/gh/romi/librcom)    
ci_dev  
![Build Status](https://github.com/romi/librcom/workflows/CI/badge.svg?branch=ci_dev)
[![codecov](https://codecov.io/gh/romi/librcom/branch/ci_dev/graph/badge.svg)](https://codecov.io/gh/romi/librcom)

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

You can now write a small application, create monster, and have them
do things.

```c++
int main(int argc, char** argv)
{
        HappyMonster monster("Elmo");
        monster.gently_scare_someone("you");
        return 0;
}
```

The full code of this example is split over the following files: [monster_simple.cpp](examples/tutorial/monster_simple.cpp), [IMonster.h](examples/tutorial/IMonster.h), and [HappyMonster.h](examples/tutorial/HappyMonster.h)

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
shown below. They mostly call upon the methods provided by
`RemoteStub`.

* Use `execute_simple_request` for methods that don't take any
  arguments and return no values.
* Use `execute_with_params` when the caller has to send arguments, but no
  return value is expected.
* Use `execute_with_result` when there are no arguments but a
  value os returned.  
* Finally, the generic method `execute` takes arguments for the remote
  method and returns a value.

Both the parameters and the return value are sent using the
[JSON](https://www.json.org/json-en.html) format. The RemoteStub takes
care of the encoding the date to a JSON string representation and
parsing the incoming string to a C++ JSON data structure. For this
rcom uses the [JSON library](https://github.com/nlohmann/json) by
Niels Lohmann. Check out its documentation to get to know all its
features.


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

The various execute methods return `true` when the remote method was
executed successfully and `false` when an error occured. They do not
throw an exception. This leaves the choice up to you whether to throw
an exception in response to a failed invokation or not. When an error
occured, the RemoteStub will write a message with to the rcom logger.

NOTE: The other functions, such as `RcomClient::create` below do throw
exceptions.

The main function for calling the remote monster is as follows. The
function `rcom::RcomClient::create` establishes the connection to a
remote object on the local network (or local machine) identified by
"elmo". The second argument is a timeout for the connection. If "elmo"
doesn't show up within 10 seconds, the application calls it quits.

If the connection is established, it is passed to the RemoteMonster
object. The application can then call the `IMonster` methods as if the
remote monster was a normal, local object.

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

The full code of the new version can be found in
[monster_client.cpp](examples/tutorial/monster_client.cpp).

## The registry

If you run the example application above, it will quit with the
following error message:

```bash
ERROR: Socket::connect: failed to bind the socket
ERROR: Socket::Socket: Failed to connect to address 192.168.1.100:10101
ERROR: main: 'Socket: Failed to connect'
```

In order for the example above to find the "elmo" object, rcom uses
another service called the rcom-registry. It is basically a directory
service the maps identifiers to IP addresses. You will have to start
the service separately:

```bash
$ ./bin/rcom-registry 
INFO: Registry server running at 192.168.1.100:10101.
```

If you run the example application again, it will still quit. This
time, after 10 seconds it will show the error message below:

```bash
WARNING: MessageLink::connect: Failed to obtain address for topic 'elmo'
ERROR: MessageLink: Failed to connect: elmo
ERROR: main: 'MessageLink: Failed to connect'
```

This is normal: we didn't implement and start the remote process, yet.

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

        void execute_jump_around();
        void execute_gently_scare_someone(nlohmann::json& params);
        void execute_get_energy_level(nlohmann::json& result);
                
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
returning large binary data. They will be discussed later.

In our example, the `execute` method checks the value of the `method`
argument and then dispatches the call to the appropriate handler: 

```c++
void MonsterAdaptor::execute(const std::string& method, nlohmann::json& params,
                             nlohmann::json& result, rcom::RPCError& error)
{
        error.code = 0;
        if (method == "jump-around") {
                execute_jump_around();
                
        } else if (method == "gently-scare-someone") {
                execute_gently_scare_someone(params);
                
        } else if (method == "get-energy-level") {
                execute_get_energy_level(result);
                
        } else {
                error.code = rcom::RPCError::kMethodNotFound;
                error.message = "Unknown method";
        }
}
```

The handlers are quite straightforward. They basically call the
corresponding methods on the "real" C++ object:

```c++
void MonsterAdaptor::execute_jump_around()
{
        monster_.jump_around();
}

void MonsterAdaptor::execute_gently_scare_someone(nlohmann::json& params)
{
        std::string id = params["person-id"];
        monster_.gently_scare_someone(id);
}

void MonsterAdaptor::execute_get_energy_level(nlohmann::json& result)
{
        result["energy-level"] = monster_.get_energy_level();
}
```

That's it! The full code of this section can be found here:
[monster_server.cpp](examples/tutorial/monster_server.cpp).

## Run the example

To run the example, you must first start the rcom-registry:

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
Raspberry Pi Zero has to transmit images, the encoding becomes a
showstopper.

So, it is therfore possible to return the data as a binary
buffer. This is the reason for the second `execute` method in the
adapter class discussed above.


TODO: example of sending binary message


The is currently no method for sending a buffer of binary data without
encoding it.



# The generic API

rcom provides both server-side and client-side websockets. We'll call
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

