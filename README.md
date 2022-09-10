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

# Documentation

The complete documentation can be found here:
[docs/README.md](docs/README.md).
