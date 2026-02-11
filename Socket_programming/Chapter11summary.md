## 11.4 The Sockets Interface

• A client and server both begin by creating a socket using the socket() function, which returns a file descriptor (clientfd on the client side and listenfd on the server side) that represents an unbound TCP endpoint.

• On the client side, clientfd is passed to connect(), which causes the kernel to implicitly bind the socket to the client’s IP address and an ephemeral port, and then initiate a TCP connection to the server’s socket address. Once connect() succeeds, clientfd is a connected socket that is ready for reading and writing.

• On the server side, the socket descriptor returned by socket() (typically called listenfd) is explicitly bound to a known local IP address and port using bind(), so that clients know where to send connection requests. The server then calls listen() to convert listenfd from an active socket into a passive listening socket that cannot initiate connections but can queue incoming connection requests.

• When a client attempts to connect, the server calls accept() on listenfd. The accept() function blocks until a connection request arrives, and then returns a new file descriptor, commonly called connfd, which represents a connected socket dedicated to communication with that specific client. The original listening socket (listenfd) remains open to accept additional connections.

• From this point forward, the client and server exchange data by reading from and writing to clientfd on the client side and connfd on the server side.

• See Figure 11.14 on page 959 for an illustration of this interaction.