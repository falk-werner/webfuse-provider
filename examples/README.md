[![Build Status](https://travis-ci.org/falk-werner/webfuse-provider.svg?branch=master)](https://travis-ci.org/falk-werner/webfuse-provider)

# webfuse-provider

Reference implementation of webfuse provider

## Build and run

To install dependecies, see below.

    meson build
    cd build
    ninja
    ./webfuse-provider

## Dependencies

- [webfuse](https://github.com/falk-werner/webfuse)
  - [libwebsockets](https://libwebsockets.org/)
  - [jansson](https://github.com/akheron/jansson)

All dependencies are bundled using meson wrap files.
