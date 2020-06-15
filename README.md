[![Build Status](https://travis-ci.org/falk-werner/webfuse-provider.svg?branch=master)](https://travis-ci.org/falk-werner/webfuse-provider)
[![codecov](https://codecov.io/gh/falk-werner/webfuse-provider/branch/master/graph/badge.svg)](https://codecov.io/gh/falk-werner/webfuse-provider)

# webfuse-provider

webfuse combines libwebsockets and libfuse. It allows ot attach a remote filesystem via websockets.

## Build and run

To install dependecies, see below.

    meson build
    cd build
    ninja
    ./webfuse-provider

## Fellow Repositories

-   **[webfuse-example](https://github.com/falk-werner/webfuse-example)**: Example of webfuse
-   **[webfuse](https://github.com/falk-werner/webfuse)**: webfuse adapter library
-   **[webfused](https://github.com/falk-werner/webfused)**: Reference implementation of webfuse daemon

## Further Documentation

-   [Webfuse Protocol](https://github.com/falk-werner/webfuse/blob/master/doc/protocol.md)
