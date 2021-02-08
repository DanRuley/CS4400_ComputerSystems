
PROGS = hostinfo selfinfo \
        udp_recv udp_send udp_recvfrom udp_from_send \
        tcp_server tcp_client tcp_server2 tcp_client2 \
        tcp_server3 tcp_client3 \
        echo

CC = gcc
CFLAGS = -Wall -O2

all: $(PROGS)

$(PROGS): csapp.c

clean:
	rm -f $(PROGS)
