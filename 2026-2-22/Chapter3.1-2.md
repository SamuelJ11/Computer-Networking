# 3.1: Introduction and Transport-Layer Services

    • A transport-layer protocol provides for logical communication between application processes running on different hosts.

    • On the sending side, the transport layer converts the application-layer messages it receives from a sending application process into transport-layer packets, known as transport-layer segments in Internet terminology.

## 3.1.1: Relationship Between Transport and Network Layers

    • A transport-layer protocol provides logical communication between processes running on different hosts, a network-layer protocol provides logical communication between hosts. 

    • Transport-layer protocols live in the end systems. Within an end system, a transport protocol moves messages from application processes to the network edge (that is, the network layer) and vice versa, but it doesn’t have any say about how the messages are moved within the network core.

    • A transport protocol can offer reliable data transfer service to an application even when the underlying network protocol is unreliable, that is, even when the network protocol loses, garbles, or duplicates packets

## 3.1.2: Overview of the Transport Layer in the Internet

    • For this book TCP and UDP packets are reffered to as segments, and the term datagram is reserverd for the network-layer packet.

    • The Internet Protocol is the Internet's network layer protocol

    • The IP service model is a best-effort delivery service. This means that IP makes its “best effort” to deliver segments between communicating hosts, but it makes no guarantees. 

    • Every host has at least one network-layer address, a so-called IP address.

    • The most fundamental responsibility of UDP and TCP is to extend IP’s delivery service between two end systems to a delivery service between two processes running on the end systems:

        - extending host-to-host delivery to process-to-process delivery is called transport-layer multiplexing and demultiplexing

## 3.2: Multiplexing and Demultiplexing

    • Recall that a process can have one or more sockets:

        - the transport layer in the receiving host does not actually deliver data directly to a process, but instead to an intermediary socket

        - for this reason, each socket has a unique identifier; the format of which depends on whether the socket is a UPD or TCP socket

    • The transport layer segment has fields that designate where a host should direct an incoming transport-layer segment

        - these special fields are the source port number field and the destination port number field, and the overall process is called demultiplexing

    • The job of gathering data chunks at the source host from different sockets, encapsulating each data chunk with header information (that will later be used in demultiplexing) to create segments, and passing the segments to the network layer is called multiplexing.

### Connectionless Multiplexing and Demultiplexing with UDP

    • For socket programming, the client side of the application lets the transport layer automatically (and transparently) assign the port number, whereas the server side of the application assigns a specific port number.

    • Suppose a process in Host A, with UDP port 19157, wants to send a chunk of application data to a process with UDP port 46428 in Host B:

        (1) the transport layer in Host A creates a transport-layer segment that includes the application data, the source port number (19157) and the destination port number (946428) along with two other values that aren't terribly important right now

        (2) the transport layer then passes the resulting segment to the network layer

        (3) the network layer encapsulates the segment in a IP datagram and makes a best-effort attempt to deliver the segment to the receiving host

        (4) the transport layer at host B examines the destination port number in the segment (46428) and delivers the segment to its socket identified by port 46428

    • It is important to note that a UDP socket is fully identified by a two-tuple consisting of a destination IP address and a destination port number

### Connection-Oriented Multiplexing and Demultiplexing with TCP

    • One subtle difference between a TCP socket and a UDP socket is that a TCP socket is identified by a four-tuple: (source IP address, source port number, destination IP address, destination port number).

        - thus, when a TCP segment arrives from the network to a host, the host uses all four values to direct (demultiplex) the segment to the appropriate socket

    • in contrast with UDP, two arriving TCP segments with different source IP addresses or source port numbers will (with the exception of a TCP segment carrying the original connection-establishment request) be directed to two different sockets, even if they have the same destination IP address and destination port number.

    • In socket programming, after the server calls accept() and the connection socket is created, this connection socket is identified by the (1) source IP address (2) source port numbeer (3) destination port in the segment, and (4) the server's IP address

        - all subsequently arriving segments whose source port, source IP address, destination port, and destination IP address match these four values will be demultiplexed to this connection socket

### Web Servers and TCP

    • Today’s high-performing Web servers often use only one process, and create a new thread with a new connection socket for each new client connection

# 3.3: Connectionless Transport: UDP


