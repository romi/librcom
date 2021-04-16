master  
![Build Status](https://github.com/romi/librcom/workflows/CI/badge.svg?branch=master)
[![codecov](https://codecov.io/gh/romi/librcom/branch/master/graph/badge.svg)](https://codecov.io/gh/libromi/librcom)    
ci_dev  
![Build Status](https://github.com/romi/librcom/workflows/CI/badge.svg?branch=ci_dev)
[![codecov](https://codecov.io/gh/romi/librcom/branch/ci_dev/graph/badge.svg)](https://codecov.io/gh/libromi/librcom)

# rcom
rcom is light-weight libary for inter-node communication. It is dependent on libr, though this will be downloaded and added to the current build path on a cmake configure, if it doesn't already exist.

All apps run as separate processes. They communicate with each other using one or more of the communication links discussed below. 

One specific app, rcregistry, maintains the list of all running apps and updates the connections when the apps start and stop. The rcregistry app should be launched before any other app. 

# NOTE: INFO BELOW IS OBSOLETE, UNDER REVIEW.

There are four type of communication between nodes:
* short real-time messages (datahubs and datalinks)
* buffered, non-realtime messages (messagehubs and messagelinks)
* one-shot, request-response exchange (services and clients)
* continuous (video) streams (streamer and streamer link)

Data messages are implemented on top of UDP and are therefore limited in size (1.5kB). Messagehub use WebSockets to pass data. Services use classical HTTP requests. Streamers use HTTP request and return the data using the multipart/x-mixed-replace format.

Although the data messages are agnostic about the content of the messages most messages are encoded in JSON. Message hubs always use JSON encoded messages. 

In addition, to those four basic link types, the rcgen utility (discussed below) recognizes the "controller" type. A controller is a messagehub that expects a command/response interaction.

All end-points have a type, a topic, and an address. The address is a combination of IP address and port number. The possible types are: datahub, datalink, messagehub, messagelink, service, streamer, streamerlink.

Both datahubs and messagehubs create one-to-many connections, with many links connected to the same hub. There can be only one messagehub for a given topic but there can be both a datahub and a messagehub with the same topic because there is no ambiguity between the two.


The rcom library does not have a "bus" type of communication but it can be built quite easily using a messagehub. 

There are several utilities:
* rcregistry: Maintains the list of all the rcom end-points, inluding their type, topic, and address. 
* rcom: The application offers a number of utility functions, inluding querying the registry, listening to the message of nodes.
* rclaunch: 
* rcgen: takes a description of an app as input and generates C code that provides a skeleton for the app.


## rcgen

```console
$ rcgen code output-file [input-file]
```

```console
$ rcgen cmakelists [output-file] [input-file]
```


### General section

The simplest input file contains the name of the app and the list communication links of the app. 

```json
{
    "name": "myapp",
    "com": []
}
```

The generated code will 
* include the header myapp.h,
* call myapp_init(argc, argv) on startup,
* call myapp_cleanup() at the end of the execution

These default values can be changed as follows:

```json
{
    "name": "myapp",
    "header": "...",
    "init": "...",
    "cleanup": "...",
    "com": []
}
```


|variable|   |default|description|function signature|
|---|---|---|---|---|
|name|required| none |Base name for the code generator|NA|
|init|optional|\<name\>_init|Name of the init function|int (*)(int argc, char **argv)|
|cleanup|optional|\<name\>_init|Name of the cleanup function|void (*)()|
|header|optional|\<name\>.h|Name of the cleanup function| |
|idle|optional|none|Name of the idle function|void (*)()|


To assure that the compiler finds the function declarations of the init, cleanup and idle function, you must add them to the header file.

### com section

The com section lists the hubs, links, and services that your app wants to create. It is a JSON array with one object for each link. 

```json
{
    "name": "myapp",
    "com": [
        {
            "type": "service",
            "topic": "servicing",
            "...": "..."
        },
        {
            "type": "messagehub",
            "topic": "hubbing",
            "..." : "..."
        }
    ]
}
```

For all the links/hubs you must specify the type and the topic name. The following types are recognized: `datahub`, `datalink`, `messagehub`, `messagelink`, `messagehub`, `controller`, `service`, `streamer`, and `streamerlink`.


#### messagelink

|variable|   |default|description|
|---|---|---|---|
|onmessage|optional| - |The callback function that handles incoming messages|
|userdata|optional| - |The pointer that will be passed to the callbacl function|

The signature of the callback function is defined in `rcom/messagelink.h`:

```c
typedef void (*messagelink_onmessage_t)(void *userdata,
                                        messagelink_t *link,
                                        json_object_t message);
```


#### messagehub

|variable|   |default|description|
|---|---|---|---|
|onconnect|optional| - |A function that is called whenever a new connection comes in|
|onmessage|optional| - |The callback function that handles incoming messages|
|onrequest|optional| - |...|
|userdata|optional| - |The pointer that will be passed to the callbacl function|
|port|optional| - |xxx|
