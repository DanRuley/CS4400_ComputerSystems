Address Resolution
------------------

The program "hostinfo.c" demonstrates how to resolve names to numeric
addresses.

UDP
---

Either of the "udp_recv.c" and "udp_recvfrom.c" programs can be used
with the "udp_send.c" and "udp_from_send.c" programs:

 * "udp_recv.c" is a basic server, which loops to receive multiple
   client messages.

 * "udp_recvfrom.c" is the same, but it reports the address of each
   client connection.

 * "udp_send.c" sends a message to a specified server.

 * "udp_from_send.c" is the same, but it accepts a specific local port
    to use before the server address, and it optionally accepts a
    repeat count to send that many messages.

TCP
---

Each TCP server variant works only with the corresponding client
variant:

 * "tcp_server.c" and "tcp_client.c" demonstrate a basic client and
   server, but with unreliable read and write strategies, so that
   when the client sends enough data (typically more than 8k), the
   server doesn't get it all before responding.

 * "tcp_server2.c" and "tcp_client2.c" use "csapp.c" functions to be
   simpler and more robust. A client explicitly closes its write
   stream to allow the server to receive an EOF, and `rio_` functions
   handle short counts automatically.

 * "tcp_server3.c" and "tcp_client3.c" use a different protocol, where
   a client tells the server in advance how many bytes it will send.
   The server doesn't have to loop, and the client doesn't have to use
   `shutdown`.
