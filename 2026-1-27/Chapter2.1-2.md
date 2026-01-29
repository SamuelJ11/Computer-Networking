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

## 2.2.1: Overview of HTTP

    • Today the vast majority of applications run over HTTP, including Web browsing, video streaming, social media, and email between clients and mail servers.

    • HTTP is implemented in two programs: a client program and a server program, which communicate by exchanging HTTP messages

    • A web page (also called a document) consists of objects:

        - an object is simply a file, such as an HTML file, a JPEG image, a Javascript file, a CCS style sheet file, or video clip, that is addressable by a single URL

    • Most web pages consist of a base HTML file and several referenced objects

        - the base HTML file references the other objects in the page with the objects’ URLs

    • Each URL has two components: the hostname of the server that houses the object and the object’s path name.

        - For example, the URL 'http://uky.edu/CS/faculty.gif' has 'www.uky.edu' as the hostname and '/CS/faculty.gif' for a path name         
            
    • Web browsers implement the client side of HTTP, so for this discussion 'browser' = 'client'.

    • Web servers implement the server side of HTTP and house Web objects.                        
    
    • HTTP defines how Web clients request Web pages from Web servers and how servers transfer these pages to clients

        - see figure 2.6 for an illustration of HTTP request-response behavior

    • First let's discuss HTTP assuming TCP is the underyling transport protocol:

        (1) HTTP initiates a TCP connection with the server
        (2) the browser and server processes access TCP through their socket interfaces
        (3) the client sends HTTP request messages into its socket interface and recieves HTTP response messages from the same interface (same with the server)
        (4) because the layered architecture of the HTTP requests relying on the underlying TCP protocol, HTTP need not worry about lost data or the details of data recovery

    • Because an HTTP server maintains no information about the clients or thier respective states, HTTP is said to be a 'stateless' protocol

    • Earlier versions of HTTP (HTTP/1.0, HTTP/1.1, and HTTP/2) run on top of TCP, but more recently HTTP/3 has been standardized [RFC 9114] and runs over UDP

## 2.2.2: Non-Persistent and Persistent Connections

    • Applications that use persistent connections are characterized by the client and server sending messages over the same TCP connection

        - non-persistent connections use seperate TCP connections for each request/response pair

### HTTP with Non-Persistent Connections

    • Basically a TCP connection is established and terminated for each object that the client requests (each non-persistent TCP connetion transports exactlyl one request message and one response message)

    • The round-trip time (RTT) is the time it takes for a small packet to travel from client to server and then back to the client (delays included)

        - the first two parts of the three-way handshake take one RTT, and the HTTP request (which comes along with the third part of the handshake) and server response makes up the other RTT

        - thus the total response time is two RTTs plus the transission time at the server of the HTML file

### HTTP with Persistent Connections

    • With persistent connections, entire Web pages can be sent over a single persistent TCP connection

        - in fact, multiple Web pages residing on the same server can be sent over the same TCP connection as these requests for objects are 'pipelined' without waiting for replies to pending requests

## 2.2.3: HTTP Message Format

    • There are two types of HTTP messages, request messages and response messages

### HTTP Request Message

    • Below we provide a typical HTTP request message:

        GET /somedir/page.html HTTP/1.1
        HOST: www.someschool.edu
        Connection: close
        User-agent: Mozilla/5.0
        Accept-language: fr

    • The first line of an HTTP request message is called the 'request line', and the subsequent lines are called the 'header lines'.

    • The request line has three fields: the method, URL and HTTP version; the requested object is identified in the URL field

    • The first header line specifies the host on which the object resides (required by Web proxy caches)

    • The second header line specifies a non-persistent connection 

    • The third header line specifies the user-agent (the browser type this making the request to the server)

    • The last header line indicates that the user prefers to receive a French version of the object (if it exists)

        * see figure 2.8 for a general format of an HTTP request message

        * here we see that the 'entity' body is empty for the GET method, but populated for the POST method

        * an important idea here is that a request generated with a form does not necessarily have to use the POST method; if GET is used the form data is put directly into the URL:

            -- for example, if a form uses the  method, has two fields, and the inputs to the two fields are 'monkeys' and 'bananas', then the URL will have the structure 'www.somesite.com/animalsearch?monkeys&bananas'

    • Other methods include:
    
        - HEAD: Like GET, but the server sends only headers, not the actual content; useful for debugging and checking metadata.

        - PUT: Uploads an object to a specific location on a Web server; commonly used by publishing tools and upload applications.

        - DELETE: Removes an object from a Web server.

### HTTP Response Message  

    • Below we provide a typical HTTP response message:

        HTTP/1.1 200 OK
        Connection: close
        Date: Mon, 21 Oct 2024 18:58:21 GMT
        Server: Apache/2.2.3 (CentOS)
        Last-Modified: Sun, 20 Oct 2024 13:20:46 GMT
        Content-Length: 6821
        Content-Type: text/html
            (data data data data data data . . .)

    • We observe that the response message has three sections: an initial status line, six header lines, and then the entity body which contains the requested object itself.

    • The status line has three fields, the protocol version, status code and a corresponding status message.

    • The first header discloses non-persistent TCP connection.

    • The second header line discloses the date when the HTTP response was created and sent by the server.

    • The third header line indicates that the message was generated by an Apache Web server.

    • The fourth header line indicates the time and date when the object was created or last modified.

    • The fifth header line indicates the number of bytes in the object being sent.

    • The last header line indicates that the object in the entity body is HTML text.

    • HTTP status codes tell the client whether a request succeeded, failed, or needs redirection:

        - 200 OK: Request succeeded; requested content is returned

        - 301 Moved Permanently: Resource has a new URL; client automatically redirects

        - 400 Bad Request: Server couldn’t understand the request

        - 404 Not Found: Requested resource doesn’t exist

        - 505 HTTP Version Not Supported: Server doesn’t support the requested HTTP version

    • Header lines are determined by the software, version, configuration, and caching behavior of the browser or server.

## 2.2.4: User-Server Interaction: Cookies

    • Cookies, defined in [RFC 6265], allow sites to keep track of users. 
    
        - most major commercial Web sites use cookies today

    • Cookie technology has four components:

        (1) a cookie header line in the HTTP response message
        (2) a cookie header line in the HTTP request message
        (3) a cookie file kept on the user's end system and managed by the user's browser
        (4) a back-end database at the Web site

        *see Figure 2.10 for an illustration of how cookie technology provides “stateful” features for HTTP

## 2.2.5: Browser Caching

    • The most recent version of HTTP (HTTP/3) has been optimized to require just one RTT to request a web page in many cases.

        - with browser caching, this can be reduced even further

    • Browser caching is the process of locally storing the content of recently received Web objects in its browser cache

        - but how does the browser know whether the object currently housed in the Web server has been modified since the copy was cached at the client?

    • The 'Cache-Control' field in an HTTP response message lets the server specify how the content in the HTTP reply message should be cached:

        - Cache-Control:no-store instructs the browser to not cache/store the content locally

        - Cache-Control:max-age = n directive tells the browser to not cache the object longer than 'n' seconds

    • The 'If-Modified-Since' field is an HTTP request header used in a conditional GET that tells the server the date and time when the browser last cached a copy of the requested object (cached timestamp).

        - if the object has been modified since that time, the server sends the updated object; otherwise, it responds without sending the object, indicating that the cached copy is still valid

        - in the case of the object not being modified since the date listed on the timestamp, then the Web server simply sends an emtpy entity body in the response message

## 2.2.6: HTTP/2

    • The primary goals for HTTP/2 (introduced in 2015) are to reduce percieved latency by enabling request and response multiplexing over a single TCP connection, provide request prioritization and server push, and provide efficient compression of HTTP header fields. 

    • Recall that HTTP/1.1 uses persistent TCP connections, but developers of Web browsers discovered that sending all the objects in a Web page over a single TCP connection has a Head of Line (HOL) blocking problem (transmission of small objects are delayed as they wait in for larger ones to finish).

        - HTTP/1.1 browsers typically work around this problem by opening multiple parallel TCP connections to reduce user-percieved delay

    • Web browsers can also "cheat" the TCP congestion control by also opening multiple parallel TCP connections to grab a larger portion of the bottleneck link bandwidth. 

    • Thus one of the primary goals of HTTP/2 is to reduce the number of parallel TCP connections for transporting single Web pages.

### HTTP/2 Framing

    • The HTTP/2 solution for HOL blocking is to break each message into small frames, and interleave the request and response messages on the same TCP connection.

        - for example, after sending one frame of a large video, the server sends the first frame of each small object, then alternates the next video frame with the remaining small-object frames—so small objects arrive much sooner

    • The ability to break down an HTTP message into independent frames, interleave them, and then reassemble them on the other end is the single most important enhancement of HTTP/2
    
    • The actual framing is done by the framing sub-layer of the HTTP/2 protocol:

        - here the header field of the response becomes one frame, and the body is broken down into one or more additional frames

        - the frames of the response are then interleaved by the framing sub-layer in the server with the frames of other responses and sent over the single persistent TCP connection

### Response Message Prioritization and Server Pushing    

    • Message prioritization allows developers to customize the relative priority of requests to better optimize application performance.

        - when a client sends concurrent requests to a server, it can prioritize the responses it is requesting by assigning a weight between 1 and 256 to each message

    • Using these weights, the server can send the frames for the responses with the highest priority first 

    • Another feature of HTTP/2 is the ability for a server to send multiple responses for a single client request:
    
        - the server can "push" additional objects to the client, without the client having to request each one     
            
        - this is possible since the HTML base page indicates the objects that will be needed to fully render the Web page

## 2.2.7: HTTP/3 and QUIC      

    • HTTP/3, standardized in 2022 in RFC 9114 runs over UDP

### The QUIC Transport Protocol

    • HTTP/3 runs over the Quick UDP Internet Connections (QUIC) transport protocol

        - strictly speaking, this isn't actually a transport layer protocol, but instead a sub-layer in the application layer that uses UDP to send and recieve packets over the Internet

### Why a New “Transport Protocol”?

    • Most HTTP/1/1 and HTTP/2 Web transactions today employ TLS running on top of TCP (see figure 2.11).

        - TLS on top of TCP is commonly known as HTTPS

    • One of the most important features of QUIC is that it gets rid of this double TCP-TLS handshake by absorbing the TLS handshake phase into the initial connection setup handshake.
              
    • QUIC also re-examines the HOL blocking problem:

        - since multiple messages are multiplexed over a single TCP connection for HTTP/2, a lost packet must be retrasmitted in accoradance with TCP, which delays all the other messages even if they don't depend on the lost packet

        - QUIC solves this by using UDP instead of TCP, and then applying reliable data-transfer to each message separately within the same QUIC connection

### QUIC Services

    • The services provided by QUIC are as follows:

        (1) built-in encryption: QUIC integrates  (which we will learn about in Chapter 8) directly into its protocol

        (2) independent data streams: QUIC allows multiple streams (for example, multiple HTTP messages) to be sent simultaneously over a single QUIC connection

        (3) 0-RTT handshakes: for returning clients, QUIC supports 0-RTT handshakes, allowing data to be sent immediately without waiting for a full connection handshake

        (4) connection migration: QUIC supports connection migration, allowing connections to remain active if the client’s IP address changes 

## HTTP/3: Running the Web over QUIC

    • The only major change as compared with HTTP/2 is that instead of using a persistent TCP connection between client and server to send the HTTP messages, HTTP/3 uses a persistent QUIC connection
    
    
