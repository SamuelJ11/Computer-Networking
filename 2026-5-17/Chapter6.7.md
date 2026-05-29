# 6.7: Retrospective: A day in the Life of a Web Page Request

    • Here we wrap up our journey down the protocol stack by taking an integrated, holistic view of the protocols we’ve learned about so far. 

## 6.7.1: Getting Started: DHCP, UDP, IP, and Ethernet

    • Suppose you are on campus and you connect an ethernet cable to the CS department's switch, which is connected to a router which is connected to the ISP.

        - here we assume the DNS server resides in the ISP's network

        - we also assume the DHCP server is running within the router (as is often the case)

    • When you first conect your laptop to the network, you can't really do anything yet since you don't have an IP address;

        - thus, the first network-related action taken by your laptop is to run the DHCP protocol to obtain an IP address from the local DHCP server 

        (1) your operating system creates a DHCP DISCOVER message and puts this message within a UDP segment with destination port 67 and source port 68

        (2) the UDP segement is then placed within an IP datagram with a broadcast address of 255.255.255.255 and a "This Host on This Network" source IP address of 0.0.0.0

        (3) the IP datagram is then placed within an Ethernet frame, which is has the destination broadcast address of FF.FF.FF.FF.FF.FF so that the frame will be broadcast to all devices connected to the switch (hopefully a DHCP server), and the frame's source MAC address is that of the laptop's NIC

        (4) the switch broadcasts the frame on all outgoing ports, including the port connected to the router

        (5) the router recieves the broadcast Ethernet frame and the IP datagram is extracted from the Ethernet frame

            * recall from section 6.4 that the type field in an Ethernet header permits Ethernet to multiplex network-layer protocols
            * the router's DCHP server's NIC looks at this field, sees the value 0x0800 (which stands for IPv4), and determines the payload belongs to IPv4. 
            * it strips the Ethernet header and passes the raw IP datagram up

        (6) the datagram’s broadcast IP destination address indicates that this IP datagram should be processed by upper layer protocols at this node, so the datagram’s payload (a UDP segment) is thus demultiplexed up to UDP, and the DHCP request message is extracted from the UDP segment

            * recall from section 6.4 that the 8-bit protocol field in an IPv4 header denotes the type of transport layer service
            * the server's operating system kernel sees the value 17 in this protocol field (which stands for UDP), and strips the IP header and passes the remaining UDP segment up the network stack

        (7) the DHCP discover message is extracted from the UDP segment and the DHCP server now has the DHCP discover message

            * the operating system kernel reads the destination port 67 in the UDP header   
            * the kernel strips away the UDP header and passes the raw payload (the DHCP DISCOVER message) directly into the memory space of the DHCP server application
            * the HDCP server chooses and temporarily reserves an IP address for your specific MAC address so it won't accidentally give it to someone else, and determines the network parameters to send back (the subnet mask, the default gateway IP, the DNS server IPs, and the lease time duration)

        (8) the DHCP server responds with a DHCP OFFER message containing the reserved network parameters - this message traverses the exact same protocol stack outlined in steps (2) through (7), but in the reverse direction

            * the DHCP server application generates the DHCP OFFER payload and hands this to the OS kernel, which encapsulates it into a UDP segment swapping the ports: source port is now 67 and destination port is now 68
            * the kernel encapulates the UDP segment into an IP datagram, but since your laptop doesn't officially own the IP address yet, the server still sets the Destination IP to the broadcast address
            * the IP datagram is placed into an Ethernet frame, and the Source MAC is the router’s interface, and the destination MAC may be either the broadcast MAC address or the client’s MAC address depending on implementation
            * because the switch is self-learning and previously received an Ethernet frame (containing the DHCP DISCOVER request) from your laptop, the switch knows to forward a frame addressed to only to the output port leading to your laptop (68) ... HOWEVER, if the router (DHCP server) chooses to send the DHCP OFFER as a Layer 2 broadcast (FF:FF:FF:FF:FF:FF), the switch must flood it out of every port, rendering its self-learned MAC table useless for that specific frame
            * your laptop receives the frame and multiplexes up the stack using the exact same logic the router used earlier in step (6)

        (9) now the client sends a broadcast DHCP request message following the same steps as (2) - (7)

            * the reason DHCP REQUEST is often still broadcast is not because the client doesn’t know the server's address
            * it’s because DHCP is designed so that all DHCP servers on the subnet can see which offer the client accepted

        (10) finally, the server responds with a unicast DHCP ACK message that contains IP address, the subnet mask, the default gateway IP, the DNS server IPs, and the lease time duration

        (11) your laptop's DHCP client then records its IP address and the IP address of its DNS server, then installs the address of the default gateway into its IP forwarding table 

            * your laptop will now forward all datagrams with destination address outside of its subnet to the default gateway

## 6.7.2: Still Getting Started: DNS and ARP

    • At this stage, your laptop needs to know the IP address of www.google.com so that your broswer can open a TCP socket to send the HTTP request to www.google.com

        - we learned in section 2.5 that the DNS protocol is used to provide this name-to-IP-address translation service

        (12) your laptop's OS creates a DNS query message by putting the string "www.google.com" in the question section of the DNS message (see section 2.4.3 for a refresher on DNS query/reply message structure)

            * this DNS message is then placed within a UDP segment witha destination port of 53 (DNS server)
            * since the DHCP OFFER and the DHCP ACK messages returned the IP address of the DNS server AND the assigned the laptop's IP address itself, the UPD segment is placed within an IP datagram with an IP destination of [DNS SERVER ADDRESS] and source IP address of [YOUR LAPTOP'S IP ADDRESS]

        (13) your IP datagram is then encapsulated into an ethernet frame and addressed to the gateway router in your school's network

            * recall the gateway IP was also obtained from the DHCP OFFER and the DHCP ACK messages
            * but there's a problem; we still dont have the MAC address of the gateway router, so your laptop has to use the ARP protocol

        (14) your laptop creates an ARP REQUEST message with a target IP of [GATEWAY ROUTER IP] and places the ARP message within an Ethernet frame with a broadcast destination address (FF.FF.FF.FF.FF.FF)

        (15) the gateway router receives the frame containing the ARP query message on the interface to the school network, and since the gateway's router IP address matches the destination IP address in the ARP packet, it prepares an ARP REPLY

            * it places the ARP reply message in an Ethernet frame, with a destination address of [YOUR LAPTOPS MAC ADDRESS] and sends the frame to the switch, which delivers the frame to your laptop

        (16) your laptop extracts the MAC address of the gateway router from the ARP REPLY message

        (17) you can (finally!) address the Ethernet frame containing the DNS query to the gateway router’s MAC address

            * note that the destination IP address of the IP datagram is that of the DNS server, but the destination MAC address of the Ethernet frame is that of the gateway router
            * your laptop sends the frame to the switch, which delivers the frame to the gateway router

## 6.7.3: Still Getting Started: Intra-Domain Routing to the DNS Server

    • Continuing to generalize upon the example given from figure 6.34, we observe that now the DNS request needs to go from the school's gateway router to the ISP's edge router

        (18) the school's gateway router recieves the frame and extracts the IP datagram containing the DNS query

            * the router looks up the destination address of this datagram (DNS server address) and determines from its forwarding table that it should be sent to the ISP's edge router
            * the IP datagram is placed inside a link-layer frame appropriate for the link connecting the school’s router to the ISP's edge router and is sent over the link

        (19) the ISP's edge router recieves the frame, extracts the IP datagram, and examines the datagrams destination address

            * because the DNS server resides inside the ISP’s own network, the edge router does not need external protocols like BGP
            * instead, the ISP's internal routing tables have been mapped by an intra-domain protocol—such as OSPF or IS-IS; this protocol calculates the shortest internal path (using Dijkstra’s least-cost path algorithm) to reach the DNS server
   
        (20) eventually the IP datagram containing the DNS query arrives at the DNS server

            * the DNS server extracts the DNS query message, lookus up the name www.google.com and finds the DNS resource record that contains the IP address  for www.google.com (assuming that it's currently cached in the DNS server, which originally was obtained from the authoritative DNS server)
            * the DNS server forms a DNS reply message containing this hostname-to-IP-address mapping and places it in a UDP segment, which is then encapsulated within an IP datagram destined for your laptop's IP address
            * the datagram is forwarded back through your ISP's network to the school's router and from there via the ethernet switch to your laptop

        (21) your laptop extracts the IP address of the server www.google.com from the DNS message and finally is now ready to contact the www.google.com server!

## 6.7.4: Web Client-Server Interaction: TCP and HTTP

    • Now that your laptop has the IP address of www.google.com, it can create the TCP socket that will be used to send the HTTP GET message to www.google.com.

        (22) first, your laptop's operating system creates a TCP socket and performs a TCP three-way handshake with the TCP/IP stack of the Google server

            * your laptop creates a TCP SYN segment with destination port 80 (for HTTP), places the TCP segment inside an IP datagram with a destination of [GOOGLE'S WEB SERVER IP ADDRESS] (which was obtained via DNS) and places the datagram inside a frame with a destination MAC of [GATEWAY ROUTER'S MAC ADDRESS] (which was obtained via ARP)
            * this frame is then sent to the switch

        (23) the routers in the school network, the ISP's network, and Google’s network forward the datagram containing the TCP SYN toward www.google.com, using the forwarding table in each router

            * recall that the router forwarding table entries governing forwarding of packets over the inter-domain link between the ISP and Google networks are determined by the BGP protocol 

        (24) eventually, the datagram containing the TCP SYN arrives at www.google.com

            * the TCP SYN message is extracted from the datagram and demultiplexed to the welcome socket associated with port 80
            * a connection socket is created for the TCP connection between the Google HTTP server and your laptop
            * a TCP SYNACK segment is generated, placed inside a datagram addressed to your laptop, and finally placed inside a link-layer frame appropriate for the link connecting www.google.com to its first-hop router

        (25) the datagram containing the TCP SYNACK segment is forwarded through the Google, ISP and school networks, eventually arriving at the Ethernet controller (NIC) in your laptop

            * the OS demultiplexes the incoming datagram to the specific TCP socket created in step 22, completing the handshake and establishing an active, ready-to-use connection
            * note that the final ACK packet rarely travels across the internet completely empty, instead the OS combines them: the final ACK flag and your actual HTTP GET request text are stuffed into the exact same physical packet

        (26) with the socket on your laptop now ready to send bytes to www.google.com, your browser creates the HTTP GET message containing the URL to be fetched:

            * note that we are assuming that we are using HTTP/1.1 or HTTP/2, not HTTP/3 which uses UDP as the underlying transport layer protocol
            * recall the format of an HTTP request message:

                GET /somedir/page.html HTTP/1.1
                HOST: www.google.com
                Connection: (close|open)
                User-agent: Mozilla/5.0
                Accept-language: fr

            * the "Connection" header line is the browser (aka User-agent) telling the server that it either does (open) or does not (close) want to bother with persistent connections

            * the HTTP GET message is then written into the socket, with the GET message becoming the payload of a TCP segment
            * the TCP segment is placed in a datagram and sent and delivered to www.google.com

        (27) now the HTTP server at www.google.com reads the HTTP GET message from the TCP socket, creates an HTTP response message, places the requested Web page content in the body of the HTTP response message, and sends the message into the TCP socket:

            * recall the format of an HTTP response message:

            HTTP/1.1 200 OK
            Connection: close
            Date: Mon, 21 Oct 2024 18:58:21 GMT
            Server: Apache/2.2.3 (CentOS)
            Last-Modified: Sun, 20 Oct 2024 13:20:46 GMT
            Content-Length: 6821
            Content-Type: text/html
                (data data data data data data . . .)

        (28) the datagram containing the HTTP reply message is forwarded through the Google, ISP, and school networks, and arrives at your laptop

            * your Web browser program reads the HTTP response from the socket, extracts the html for the Web page from the body of the HTTP response, and finally (finally!) displays the Web page!