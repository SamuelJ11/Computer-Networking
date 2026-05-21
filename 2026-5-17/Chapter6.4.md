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

        - since this host does not have an entry in the ARP table, so the sender contructs an ARP packet, which has several fields including teh sending and recieving IP and MAC addresses

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

    • 


        



        