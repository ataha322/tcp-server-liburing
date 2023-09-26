# Asynchronous TCP server using C++ and Linux kernel IO interface io_uring via liburing #

    * build -  `cd build && cmake .. && make`

    * run - `./tcp-server 8080` or your port of choice.

    * connect - `telnet 0.0.0.0 8080` or `telnet localhost 8080`

## Specification ##

    * Supports numerous connections asynchronously without any threading,
    only through io_uring requests.

    * Received messages are written into `build/<port_num>.txt`.

    * For each message the server replies with `ACCEPTED` delayed by 3 seconds.
    
    * Each message is delayed asynchronously using liburing, regardless of any other
    messages or connections.

    * Server shuts down when the last connection is closed.


