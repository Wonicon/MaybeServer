# A HTTP Server(?) that only supports GET...

Used to learn socket programming...

## Build

```
git clonse https://github.com/Wonicon/MaybeServer.git
cd MaybeServer
make
```

## Usage

The default exectuable filename is `server`.

```
./server root-path port-number
```

## Reference

http://www.linuxhowtos.org/C_C++/socket.htm

This site shows basic client and server code, with detailed explanation.
The way it initializes `sockaddr_in` structure and uses socket-related system calls is really concise.

http://blog.abhijeetr.com/2010/04/very-simple-http-server-writen-in-c.html

Thanks to this example, I realised that I should call `shutdown`, but not only `close` to destroy sockets.
Moreover, its way to send requested file is much easier, directly opening the file with 'open' system call.

