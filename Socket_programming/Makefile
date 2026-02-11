CC = gcc
CXXFLAGS = -Wall -g

CLIENTOBJECTS = TCPEchoClient.o DieWithError.o
SERVEROBJECTS = TCPEchoServer.o DieWithError.o HandleTCPClient.o

CLIENTSOURCES = TCPEchoClient.c DieWithError.c
SERVERSOURCES = TCPEchoServer.c DieWithError.c HandleTCPClient.c

CLIENTEXEC = client
SERVEREXEC = server

all: $(CLIENTEXEC) $(SERVEREXEC)

$(CLIENTEXEC): $(CLIENTOBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

$(SERVEREXEC): $(SERVEROBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o 

