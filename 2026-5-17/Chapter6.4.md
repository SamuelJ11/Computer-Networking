# 6.4: Switched Local Area Networks

    • Instead of using IP addresses, switches use link-layer addresses to forward link-layer frames through the network of switches.

## 6.4.1: Link-Layer Addressing and ARP

### MAC Addresses

    • In truth, it is not hosts and routers that have link-layer addresses but rather their adapters (that is, network interfaces) that have link-layer addresses. 

        - it’s important to note, however, that link-layer switches do not have link-layer addresses associated with their interfaces that connect to hosts and routers.

        * hosts dont send frames to the switch's MAC address; the switch simply forwards them

    • It should be obvious by now that a link-layer address is called a MAC address (also sometimes called a LAN or physical address)

        - because MAC address are 6 hexadecimal bytes, there are 2⁴⁸ possible MAC addresses

        - although MAC addresses were designed to be permanent, it is now possible to change an adapter’s MAC address via software.

    • The IEEE ensures that manufacturers assign globally unique MAC address for each piece of hardware:

        - when a company wants to manufacture adapters, it purchases a chunk of the address space consisting of 2²⁴ addresses for a nominal fee. IEEE allocates the chunk of these addresses by fixing the first 24 bits of a MAC address and letting the company create unique combinations of the last 24 bits for each adapter

    • When an adapter wants to send a frame to some destination adapter, the sending adapter inserts the destination adapter’s MAC address into the frame and then sends the frame into the LAN:

        - thus, when an adapter receives a frame, it will check to see whether the destination MAC address in the frame matches its own MAC address 

    • However, sometimes a sending adapter does want all the other adapters on the LAN to receive and process the frame it is about to send;
    
      -  in which case the sending adapter insersts a special MAC broadcast address into the destination address field of the frame (this is donated as FF-FF-FF-FF-FF-FF)

### Address Resolution Protocol (ARP)

    • Because there are both network-layer addresses and link-layer addresses, there is a need to translate between them. 

        - for the Internet, this is the job of the Address Resolution Protocol (ARP)

    • Lets consider an example to kick off our discussion of ARP:

        - suppose a host with IP address 222.222.222.220 wants to send an IP datagram to host 222.222.222.222

        - to send a datagram, the source must give its adapter not only the IP datagram but also the MAC address for the destination; but how does the sending host determine this MAC address? Enter ARP ...

        - an ARP module in the sending host takes an IP address on the same LAN (subnet) as input and returns the corresponding MAC address

    • Now lets discuss how ARP actually works:

        - each host and router has an ARP table in its memory, which contains mappings of IP addresses to MAC addresses (see the ARP table below):

            +-----------------+-------------------+----------+
            |   IP Address    |    MAC Address    |   TTL    |
            +-----------------+-------------------+----------+
            | 222.222.222.221 | 88-B2-2F-54-1A-0F | 13:45:00 |
            | 222.222.222.223 | 5C-66-AB-90-75-B1 | 13:52:00 |
            +-----------------+-------------------+----------+

        * this is an example of what an ARP table in host 222.222.222.220 might look like
        * a typical expiration time for an entry is 20 minutes from when an entry is placed in an ARP table

        - now suppose that the same host wants to send a datagram that is IP-addressed to another host or router on that subnet, lets say to host 222.222.222.222

        - since this host does not have an entry in the ARP table, the sender contructs an ARP packet, which has several fields including the sending and recieving IP and MAC addresses

        - this ARP query packet is passed to the adapter long with an indication that the adapter should send the packet to the MAC broadcast address, namely FF-FF-FF-FF-FF-FF

        - the frame containing the ARP query is received by all the other adapters on the subnet, and (because of the broadcast address) each adapter passes the ARP packet within the frame up to its ARP module

        * each of these ARP modules checks to see if its IP address matches the destination IP address in the ARP packet, and the one with a match sends back to the querying host a response ARP packet with the desired mapping

    • Interestingly, the query ARP message is sent within a broadcast frame, whereas the response ARP message is sent within a standard frame.

### Sending a Datagram off the Subnet

    • The question that now remains is how a sender can send a network-layer datagram to a host off the subnet.

    • Referencing figure 6.19 we see that all interfaces connected to subnet 1 have addresses of the form 111.111.111.xxx and for subnet 2 it's 222.222.222.xxx:

        - if host 111.111.111.111 wants to send an IP datagram to host 222.222.222.222 by using the MAC address of the destination host, then none of the adapters on subnet 1 would bother to pass it up to the network layer since the frame’s destination address would not match the MAC address of any adapter on subnet 1

        - the correct procedure is to actually send the datagram from host 111.111.111.111 to the router interface for that subnet, which has a MAC address of E6-E9-00-17-BB-4B

        * of courese the sending host obtains the MAC address for 111.111.111.110 (router's network address) via ARP!

        - for the datagram to go from the router to subnet 2, the router now has to determine the correct interface on which the datagram is to be forwarded

        * as discussed in chapter 4.2.1, this is done by consulting a forwarding table in the router:

            +----------------------------+---------------+
            |          Prefix            | Link Interface|
            +----------------------------+---------------+
            | 11001000 00010111 00010    |       0       |
            +----------------------------+---------------+
            | 11001000 00010111 00011000 |       1       |
            +----------------------------+---------------+
            | 11001000 00010111 00011    |       2       |
            +----------------------------+---------------+
            | Otherwise                  |       3       |
            +----------------------------+---------------+

            example forwarding table from section 4.2.1 

        - now the destination MAC address of the frame is indeed the MAC address of the ultimate destination. And how does the router obtain this destination MAC address? From ARP, of course!

## 6.4.2: Ethernet

### Ethernet Frame Structure

    • Below is the copied figure 6.17 of an ethernet frame:

        +-----------+---------------+---------------+-----------+-----------------------+-----------+
        | Preamble  | Dest. address | Source address|   Type    |         Data          |    CRC    |
        +-----------+---------------+---------------+-----------+-----------------------+-----------+
        |  8 Bytes  |    6 Bytes    |    6 Bytes    |  2 Bytes  |   46 to 1500 Bytes    |  4 Bytes  |
        +-----------+---------------+---------------+-----------+-----------------------+-----------+
                                                                \_______________________/
                                                                            |
                                                                Contains Layer 3 IP payload

    • From the figure, we note several important facts:

        (1) the maximum transmission unit (MTU) of Ethernet is 1500 bytes. if the IP datagram is less than 46 bytes, the data field has to be "stuffed" to fill out the 46 bytes. 

            * for example, recall the IPv4 Header diagram from figure 4.17:

            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |Version| Header|Type of service|    Datagram length (bytes)      |
            |       | length|               |                                 |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |       16-bit Identifier       |Flags|13-bit Fragmentation offset|
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |  Time-to-live |  Upper-layer  |        Header checksum          |
            |               |    protocol   |                                 |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |                     32-bit Source IP address                    |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |                  32-bit Destination IP address                  |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |                        Options (if any)                         |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |                                                                 |
            |                              Data                               |
            |                                                                 |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

            * let's say the at the application layer, the payload is 20 bytes
            * recall from section 3.5 that the transport layer (assuming we're using TCP) has an additional 20-byte header (recall from section 3.3 that if this were UDP, the header would only be 8 bytes)
            * at the network layer, right before IP hands anything to the Ethernet hardware, the IP software looks at the chunk it is holding. it counts the bytes: 20 bytes of TCP header + 20 bytes of application data = 40 bytes
            * the IP software adds its own 20-byte IP header to the front. inside that IP header, at the exact spot reserved for "Datagram length", the sender's OS explicitly writes the binary for the number 40
            * now, the IP layer hands this 60-byte block down to the physical Ethernet card. the Ethernet card has a strict hardware rule: "the data field I am handed must be at least 46 bytes long." it looks at the PAYLOAD (NOT the IP header "Datagram Length" field) and sees it's only 40 bytes
            * the ethernet card blindly appends 6 bytes of zero padding to the very end of the payload so it satisfies the physical wire requirements
            * in the recieving IP layer, because the sender explicitly wrote 40 in the "Datagram Length" field, the receiver reads it and says: "the sender told me the payload inside this packet is exactly 40 bytes."
            * the receiver measures out exactly 40 bytes right after the IP header, and it completely ignores the 6 bytes of garbage padding (stuffing) left over at the end

        (2) the destination address contains the MAC address of the destination adapter

        (3) the source address contains the MAC address of the adapter that transmits the frame onto the LAN

        (4) the type field permits Ethernet to multiplex network-layer protocols (IP, AppleTalk, Novell IPX, ARP); analogous to the protocol field in the network layer datagram

        (5) the cyclic redundancy check field (discussed in section 6.2) exits to allow the recieving adapter to detect bit errors in the frame

        (6) the preamble's first seven bytes for ethernet have a value of 10101010 and serve to "wake up" the recieving adapters and to ynchronize their clocks to that of the sender’s clock; the last byte is 10101011 (the last two bits of the eighth byte of the preamble alerts the recieving adapter that the "important stuff" is about to come)

    • Ethernet technologies provide an unreliable service to the network layer.

        - this lack of reliable transport (at the link layer) helps to make Ethernet simple and cheap, but it also means that the stream of datagrams passed to the network layer can have gaps (lets hope TCP is being used at the reciever's transport layer!)

## 6.4.3: Link-Layer Switches

    • We note that the switch itself is transparent to the hosts and routers in the subnet; that is, a host/router addresses a frame to another host/router (rather than addressing the frame to the switch) and happily sends the frame into the LAN, unaware that a switch will be receiving the frame and forwarding it. 

### Forwarding and Filtering

    • Filtering is the switch function that determines whether a frame should be forwarded to some interface or should just be dropped.

    • Forwarding is the switch function that determines the interfaces to which a frame should be directed, and then moves the frame to those interfaces. 

    • Both filtering and forwarding are done with a switch table;

        - the switch table contains entries for some, but not necessarily all, of the hosts and routers on a LAN:

        * copied from figure 6.22, a typical switch table has entries of the form:

            +-------------------+-----------+------+
            |      Address      | Interface | Time |
            +-------------------+-----------+------+
            | 62-FE-F7-11-89-A3 |     1     | 9:32 |
            | 7C-BA-B2-B4-91-10 |     3     | 9:36 |
            |     ........      |    ...    |  ... |
            +-------------------+-----------+------+

    • To understand how switch filtering and forwarding work, consider the following example:

        - suppose a frame with destination address DD-DD-DD-DD-DD-DD arrives at the switch on interface x

        - the switch indeexes its table with teh MAC address DD-DD-DD-DD-DD-DD, and there are three possible cases:

            (1) there is no entry for DD-DD-DD-DD-DD-DD; in this case the switch broadcasts the frame

            (2) there is an entry that associates DD-DD-DD-DD-DD-DD with an interface x; in this case; in this case the frame is coming from a LAN segment taht contains adapter DD-DD-DD-DD-DD-DD (aka frame source = frame destination) so the switch executes the filtering functions and discards the frame

            (3) there is an entry that associates DD-DD-DD-DD-DD-DD with an interface y != x; in this case, the frame needs to be forwarded to the LAN segment attached to interface 'y'

    • Referring back to figure 6.15, suppose that a frame with destination address 62-FE-F7-11-89-A3 arrives at the switch from interface 1 (EE department);

        * in this case, frame source = frame destination so the switch drops the frame as it has already been broadcast on the LAN segment that contains the destination

        - now suppose a frame with the same destination address arrives from interface 2 (CS department); here the switch forwards the frame to the output buffer preceding interface 1

    • The secret to how the switch even populates its switch table in the first place is due to self-learning.

### Self-Learning

    • Self learning is accomplished as follows;

        - for each incoming frame received on an interface, the switch stores in its table (1) the MAC address in the frame’s source address field, (2) the interface from which the frame arrived, and (3) the current time; if every host in the LAN eventually sends a frame, then every host will eventually get recorded in the table
              
        - the switch deletes an address in the table if no frames are received with that address as the source address after some period of time (the aging time)

### Properties of Link-Layer Switching

    • We can identify several advantages of using switches, rather than broadcast links such as buses or hub-based star topologies:

        (1) elimination of collisions; the switches buffer frames and never transmit more than one frame on a segment at any one time

        (2) heterogenous links; because a switch isolates one link from another, the different links in the LAN can operate at different speeds and can run over different media

        (3) management; if an adapter malfunctions and continually sends Ethernet frames (called a jabbering adapter) for example, a switch can detect the problem and internally disconnect the malfunctioning adapter.

### Switches Versus Routers

    • Given that both switches and routers are candidates for interconnection devices, what are the pros and cons of the two approaches?

        SWITCHES:

        - switches are plug and play and can also have relatively high filtering and forwarding rates

        - to understand the discussion below, it is important to briefly expose ourselves to the Spanning Tree Protocol:

        * Spanning Tree Protocol (STP)

            when switches are turned on and connected together, they automatically send control packets back and forth to map out the physical layout of the network. they elect one central switch as the "Root" of the tree, and then calculate the shortest path to that root from every other switch

            if STP discovers a loop (a redundant link), it dynamically forces one of the physical ports on that path into a Blocking State

            logically the loop is broken; traffic can no longer circle endlessly

            physically the cable is still plugged in, acting as a standby. if the primary link goes down, STP detects the failure within seconds and automatically unblocks the backup port to restore connectivity

        - now we understand that in a switched network, to prevent the cycling of broadcast frames (known as a broadcast storm), the active topology of a switched network is restricted to a spanning tree

        - additionally, a large switched network would require large ARP tables in the hosts and routers and would generate substantial ARP traffic and processing

        ROUTERS

        - packets are not restricted to a spanning tree and can use the best path between source and destination

        - they also provide firewall protection against layer 2 broadcast storms

        - routers and the hosts that connect to them need their IP addresses to be configured

        - also, routers often have a larger per-packet processing time than switches, because they have to process up through the layer 3 fields

    • Typically, Switches suffice for small networks consisting of a few hundred hosts as they localize traffic and increase aggregate throughput without requiring any configuration of IP addresses.

    • Larger networks consisting of thousands of hosts typically include routers within the network (in addition to switches) as they provide a more robust isolation of traffic, control broadcast storms, and use more “intelligent” routes among the hosts in the network.

## 6.4.4: Virtual Local Area Networks (VLANs)

    • Three drawbacks can be identified in the configuration in figure 6.15:

        (1) lack of traffic isolation: although the hierarchy localizes group traffic to within a single switch, broadcast traffic (e.g., frames carrying ARP and DHCP messages) must still traverse the entire institutional network

        (2) inefficient use of switches: if instead of three groups, the institution had ten groups, then ten first-level switches would be required.  also if each group were small, say less than 10 people, then a single 96-port switch would likely be large enough to accommodate everyone, but this single switch would not provide traffic isolation

        (3) managing users: if an employee moves between groups, the physical cabling must be changed to connect the employee to a different switch (employees belonging to two groups make the problem even harder)

    • Fortunately, each of these difficulties can be handled by a switch that supports virtual local area networks(VLANs):

        - as the name suggests, a switch that supports VLANs allows multiple virtual local area networks to be defined over a single physical local area network infrastructure

    • In a port-based VLAN, the switch’s ports (interfaces) are divided into groups by the network manager. 

        - each group constitutes a VLAN, with the ports in each VLAN forming a broadcast domain (i.e., broadcast traffic from one port can only reach other ports in the group)

        - the network manager declares a port to belong to a given VLAN using switch management software, and a table of port-to-VLAN mappings is maintained within the switch; and switch hardware only delivers frames between ports belonging to the same VLAN

    • VLAN traffic isolation comes with an additional difficulty: how can traffic from one department be sent to the other?

        - in modern practice, the most scalable approach to interconnecting VLAN switches is known as VLAN trunking (see figure 6.26b)

        - in this approach, a special port on each switch (port 16 on the left switch and port 1 on the right switch) is configured as a trunk port to interconnect the two VLAN switches

        - the trunk port belongs to all VLANs, and frames sent to any VLAN are forwarded over the trunk link to the other switch

        * how does a switch know that a frame arriving on a trunk port belongs to a particular VLAN?

            - IEEE defined teh extended Ethernet frame format, 802.1Q, for frames corssing a VLAN trunk:

            +----------+----------+----------+------------+------------+------+----------+-----+-----+
            | Preamble |  Dest.   |  Source  | Tag Proto. |Tag Control | Type |   Data   | ... | CRC |
            |          | address  | address  | Identifier |Information |      |          |     |     |
            +----------+----------+----------+------------+------------+------+----------+-----+-----+
                                             \_________________________/
                                                        |
                                                 4-Byte VLAN Tag
                                                 (Added by Trunk)


            -----------+---------------+---------------+-----------+-----------------------+-----------+
            | Preamble  | Dest. address | Source address|   Type    |         Data          |    CRC    |
            +-----------+---------------+---------------+-----------+-----------------------+-----------+
                                                                    \_______________________/
                                                                                |
                                                                    Contains Layer 3 IP payload

            - the top ascii diagram depicts the 802.1Q tagged Ethernet VLAN frame, while the bottom is the standard ethernet frame structure given again for comparison

            - the four-byte VLAN tag added into the header carries the identity of the VLAN to which the frame belongs

            - the VLAN tag itself consists of a 2-byte Tag Protocol Identifier (TPID) field (with a fixed hexadecimal value of 0x8100), a 2-byte Tag Control Information field that contains a 12-bit VLAN identifier field, and a 3-bit priority field that is similar in intent to the IP datagram TOS field