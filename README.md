master  
![Build Status](https://github.com/romi/librcom/workflows/CI/badge.svg?branch=master)
[![codecov](https://codecov.io/gh/romi/librcom/branch/master/graph/badge.svg)](https://codecov.io/gh/romi/librcom)    
ci_dev  
![Build Status](https://github.com/romi/librcom/workflows/CI/badge.svg?branch=ci_dev)
[![codecov](https://codecov.io/gh/romi/librcom/branch/ci_dev/graph/badge.svg)](https://codecov.io/gh/romi/librcom)

# rcom
rcom is light-weight libary for inter-node communication. It is dependent on libr, though this will be downloaded and added to the current build path on a cmake configure, if it doesn't already exist.

All apps run as separate processes. They communicate with each other using one or more of the communication links discussed below. 

One specific app, rcregistry, maintains the list of all running apps and updates the connections when the apps start and stop. The rcregistry app should be launched before any other app. 
