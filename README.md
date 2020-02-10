[![Build Status](https://travis-ci.org/falk-werner/webfuse-provider.svg?branch=master)](https://travis-ci.org/falk-werner/webfuse-provider)

# webfuse-provider

Reference implementation of webfuse provider

## Build and run

To install dependecies, see below.

    cd webfused
    mkdir build
    cd build
    cmake ..
    make
    ./webfuse-provider

## Dependencies

- [webfuse](https://github.com/falk-werner/webfuse)
  - [libwebsockets](https://libwebsockets.org/)
  - [jansson](https://github.com/akheron/jansson)

### Installing dependencies

#### libwebsockets

    wget https://github.com/warmcat/libwebsockets/archive/v3.2.0.tar.gz -O libwebsockets.tar.gz
    tar -xf libwebsockets.tar.gz
    cd libwebsockets-3.2.0
    mkdir .build
    cd .build
    cmake ..
    make
    sudo make install


#### jansson

    wget https://github.com/akheron/jansson/archive/v2.12.tar.gz -O jansson.tar.gz
    tar -xf jansson.tar.gz
    cd jansson-2.12
    mkdir .build
    cd .build
    cmake ..
    make
    sudo make install

#### webfuse

    wget https://github.com/falk-werner/webfuse/archive/master.tar.gz -O webfuse.tar.gz
    tar -xf webfuse.tar.gz
    cd webfuse-master
    mkdir .build
    cd .build
    cmake -DWITHOUT_ADAPTER=ON ..
    make
    sudo make install
