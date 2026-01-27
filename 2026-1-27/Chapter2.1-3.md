# Chapter 2.1: Principles of Network Applications

    • When you build a network application, you only write programs that run on end devices (clients and servers); the network itself just moves data and you don’t program it.

## 2.1.1: Network Application Architectures

    • When developing a network application, there are two choices of architectural paradigms to choose from:

        (1) client-server architecture - an always-on host (server) services requests from many other hosts called clients

            - clients do not directly communicate with each other, traffic is directed through the server

            - server has a fixed, well-known IP address (e.g., 8.8.8.8 for Cloudflare's DNS server)

            - data centers housing a large number of hosts running virtual servers is often used to accomodate many hosts simultaneously

        (2) peer-to-peer (P2P) architecture - applications exploit direct communication between pairs of intermittently connected hosts, called 'peers'

            - P2P architectures are self-scalable and cost effective, but they face challenges of security, performance and reliablility due to thier decentralized structure

## 2.1.2: Process Communicating

    • Recall that in the jargon of operating systems, it is not the programs but processes that communicate

        - interprocess communication defines processes communicating on the same host

        - processes on two different end systems, however, communicate with each other by exchanging messages across the network

### Client and Server Processes

    • In the realm of web applications, the browser is a clinet process and the web server is a server process

        - more generally, the process that initiates the communication is the client, and the one that waits to be contacted to begin the session is the server

### The Interface Between the Process and the Computer Network

    • A process sends messages into, and receives messages from, the network through a software interface called a 'socket',

    • A process is analogous to a house and its socket is analogous to its door:
    
        - when a process wants to send a message to another process on another host, it shoves the message out its door (socket)

    • Crucially, a socket is the interface between the application layer and the transport layer within a host. 
    
        - it is also referred to as the Application Programming Interface (API) between the application and the network, since the socket is the programming interface with which network applications are built

### Addressing Processes

    • In the Internet, the host is identified by its IP address

    • The recieving process (socket) running in the host is also needed to identify the receiving process among potentially many network applications running on the host:
    
        - A destination port number servers this purpose

## 2.1.3: Transport Services Available to Applications

    • When you develop an application, you must choose one of the available transport-layer protocols. 

    • We can broadly classify the possible services provided by transport protocols along four dimensions: reliable data transfer, throughput, timing, and security.

### Reliable Data Transfer

    • One important service that a transport-layer protocol can potentially provide to an application is process-to-process reliable data transfer

        -  the sending process can just pass its data into the socket and know with complete confidence that the data will arrive without errors at the receiving process

    •  loss-tolerant applications, most notably multimedia applications such as conversational audio/video, can tolerate some data loss without crucial impairment

### Throughput

    • Another natural service that a transport-layer protocol could provide is guaranteed available throughput at some specified rate.

    • Applications that have throughput requirements are said to be bandwidth-sensitive applications.

    • Elastic applications can make use of as much, or as little, throughput as happens to be available.

        - electronic mail, file transfer and web transfers are all elastic applications

### Timing

    • A transport-layer protocol can also provide timing guarantees.

        - such a service would be appealing to interactive real-time applications, such as Internet telephony, video conferencing, metaverses, and multiplayer games, all of which require tight timing constraints on data delivery in order to be effective

### Security

    • Finally, a transport protocol can provide an application with one or more security services.

        - such a service would provide confidentiality between the two processes, even if the data is somehow observed between sending and receiving processes (e.g., TLS, but this an enhancement of TCP, not another transport protocol)

## 2.1.4: Transport Services Provided by the Internet

    • The Internet (and, more generally, TCP/IP networks) makes two transport protocols available to applications, UDP and TCP

    • When an application developer create a new network application for the Internet, one of the first decisions they have to make is whether to use UDP or TCP

### TCP Services

    • TCP provides both a connection-oriented service and a reliable data transfer service:

        (1) connection-oriented service - this is the 3-way handshake that preceeds the TCP connection, which establishes a full-duplex connection allowing both devices to send messages at the same time

        (2) reliable data transfer service - the communicating processes can rely on TCP to deliver all data sent without error and in the proper order

    • TCP also includes a congestion-control mechanism that throttles a sending process (client or server) when the network is congested

### UDP Services

    • UDP is connectionless, so there is no handshaking before the two processes start to communicate. 
    
    • UDP also provides an unreliable data transfer service—that is, when a process sends a message into a UDP socket, UDP provides no guarantee that the message will ever reach the receiving process.

        - furthermore, messages that do arrive at the receiving process may arrive out of order

### Services Not Provided by Internet Transport Protocols

    • Today’s Internet can often provide satisfactory service to time-sensitive applications, but it cannot provide any timing or throughput guarantees.

    • Because many firewalls are configured to block (most types of) UDP traffic, Internet telephony applications often are designed to use TCP as a backup if UDP communication fails.

## 2.1.5: Application-Layer Protocols

    • An application-layer protocol defines how an application’s processes, running on different end systems, pass messages to each other.

    • Some application-layer protocols are specified in RFCs and are therefore in the public domain.

        - e.g., the Web’s application-layer protocol, HTTP (the HyperText Transfer Protocol [RFC 7230]), is available as an RFC
        
        - if a browser developer follows the rules of the HTTP RFC, the browser will be able to retrieve Web pages from any Web server that has also followed the rules of the HTTP RFC

            -- this is why websites don't care what browser you use; both browsers and web servers follow HTTP rules

    • Many other application-layer protocols are proprietary and intentionally not available in the public domain

        - e.g., Zoom uses proprietary application-layer protocols

    • It is important to distinguish between network applications and application-layer protocols:

        - The Web’s application-layer protocol, HTTP, defines the format and sequence of messages exchanged between browser and Web server
        
            -- thus, HTTP is only one piece (albeit, a critical piece) of the Web application

# Chapter 2.2: The Web and HTTP