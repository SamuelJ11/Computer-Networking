# 4.3: Overview of Network Layer

## 4.3.1: IPV4 Datagram Format

    • The key fields in the IPv4 datagram are the following:

        (1) version number: these 4 bits specify the IP protocol version of the datagram (IPv4 vs IPv6, for example).

        (2) header length: thse 4 bits are needed to determine where in the IP datagram the payload actaully begins

        (3) type of service: allows different types of IP datagrams to to be distinguished from each other (e.g., real-time vs non-real-time traffic).  recall from section 3.7 that two of the TOS bits are used for Explicit Congestion Notificaiton

        (4) datagram length: this 16-bit field denotes the total length of the IP datagram (header plus data), measured in bytes (datagrams are rarely larger than 1500 bytes)

        (5) Identifier, flages, fragmentation offset: these 3 fields have to do with so-called IP fragmentation, when a large IP datagram is broken into several smaller IP datagrams which are then forwarded independently to the destination (IPv6 does not allow for fragmentation)

        (6) time-to-live: this field is included to ensure that datagrams do not circulate forever in the network (decremented by one each time a datagram is processed by a router, and if it reaches 0 the router drops that datagram)

        (7) protocol: used only when an IP datagram reaches its final destination, indicates the specific transport-layer protocol to which the data portion of this IP datagram should be passed (6 is TCP, 17 is UDP)

            *IMPORTANT: the protocol number in the IP datagram has a role that is analogous to the role of the port number field in the transport-layer segment. The protocol number is the glue that binds the network and transport layers together, whereas the port number is the glue that binds the transport and application layers together

        (8) header checksum: this field aids a router in detecting bit errors in a received IP datagram, computed by treating each 2 bytes in the header as a number and summing these numbers using 1s complement arithmetic (see section 3.3 for a recap example on how this is computed).  note that the checksum must be recomputed and stored again at each router, since the TTL field (an possibly the options field).  

            * IMPORTANT: only the IP header is checksummed at the IP layer, while the TCP/UDP checksum is computed over the entire TCP/UDP segment

        (9) source and destination IP addresses: this is pretty straightforward, but one thing to note here is that often the source host determines the destination address via a DNS lookup

        (10) options: allows an IP header to be extended, but complicates matters for IPv4 addressing as IP datagrams can now be of variable length (for this reason, IPv6 does not include an options field)

        (11) data (payload): in most circumstances, the data field of the IP datagram contains the transport-layer segment (TCP or UDP) to be delivered to the destination

    • Note that an IP datagram has a total of 20 bytes of header (assuming no options); If the datagram carries a TCP segment, then each datagram carries a total of 40 bytes of header (20 bytes of IP header plus 20 bytes of TCP header) along with the application-layer message.

        - when an IP datagram carries a TCP segment, the TCP header is inside the IP datagram’s data portion

## 4.3.2: IPV4 Addressing

    • Just to clear up some terminology before we begin, the boundary between the host and the physical link is called an interface.

    • Because every host and router is capable of sending and receiving IP datagrams, IP requires each host and router interface to have its own IP address:
    
        - thus, an IP address is technically associated with an interface, rather than with the host or router containing that interface.
  
    • Each IP address is 32 bits long (equivalently, 4 bytes), and there are thus a total of 2³² (or approximately 4 billion) possible IP addresses.

    • These addresses are typically written in so-called dotted-decimal notation, in which each byte of the address is written in its decimal form and is separated by a period.

        - e.g., 192.32.216.9 has the binary representation as

            11000001 00100000 11011000 00001001

    • Each interface on every host and router in the global Internet must have an IP address that is globally unique:

        - a portion of an interface’s IP address will be determined by the subnet to which it is connected

    • A network interconnecting multiple host interfaces and one router interface is called a subnet.

        - IP addressing assigns an address to this subnet, for example:

            223.1.1.0/24 (where /24 is the subnet mask) indicates that the leftmost 24 bits of the 32-bit address define the subnet address

    • For a general interconnected system of routers and hosts, we can use the following recipe to define the subnets in the system:

        - to determine the subnets, detach each interface from its host or router, creating islands of isolated networks, with interfaces terminating the end points of the isolated networks 
         
        - each of these isolated networks is called a subnet

    • The Internet’s address assignment strategy is known as Classless Interdomain Routing (CIDR)

        - CIDR generalizes the notion of subnet addressing
  
    • As with subnet addressing, the 32-bit IP address is divided into two parts and again has the dotted-decimal form a.b.c.d/x where x indicates the number of bits in the first part of the address:

        - mathematically speaking, subnetting is a restricted version of CIDR that operates on fixed network classes (A, B, and C)
        
        - the 'x' most significant bits of an address constitute the network portion of the IP address (often referred to as the "prefix" of the address)

        - only these 'x' leading prefix bits are considered by routers outside the organization's network, considerably reducing the size of the forwarding tables in these routers

        - the remaining (32 - x) bits of an address can be thought of as distinguishing among the devices within a network (all of which share the same network prefix)
         
    • Before CIDR was adopted, the network portions of an IP address were constrained to be 8, 16 or 24 bits in length, an addressing scheme known as classful addressing, since subnets with 8, 16, and 24 subnet addresses were known as class A, B, and C networks, respectively.

    • When a host sends a datagram with destination address 255.255.255.255 the message is delivered to all hosts on the same subnet. Routers optionally forward the message into neighboring subnets as well (although they usually don’t).

### Obtaining a Block of Addresses

    • In order to obtain a block of IP addresses for use within an organization’s subnet, a network administrator might first contact its ISP, which would provide addresses from a larger block of addresses that had already been allocated to the ISP. 
    
        - for example, the ISP may itself have been allocated the address block 200.23.16.0/20, and he ISP, in turn, could divide its address block into eight equal-sized contiguous address blocks (200.23.16.0/23) and give one of these address blocks out to each of up to eight organizations that are supported by this ISP
  
        +----------------+-----------------+------------------------------------------+
        | ISP's block:   | 200.23.16.0/20  | 11001000 00010111 00010000 00000000      |
        |                |                 | -------- -------- ----                   |
        +----------------+-----------------+------------------------------------------+
        | Organization 0 | 200.23.16.0/23  | 11001000 00010111 00010000 00000000      |
        |                |                 | -------- -------- -------                |
        +----------------+-----------------+------------------------------------------+
        | Organization 1 | 200.23.18.0/23  | 11001000 00010111 00010010 00000000      |
        |                |                 | -------- -------- -------                |
        +----------------+-----------------+------------------------------------------+
        | Organization 2 | 200.23.20.0/23  | 11001000 00010111 00010100 00000000      |
        |                |                 | -------- -------- -------                |
        +----------------+-----------------+------------------------------------------+
        |      ...       |                 |                   ...                    |
        +----------------+-----------------+------------------------------------------+
        | Organization 7 | 200.23.30.0/23  | 11001000 00010111 00011110 00000000      |
        |                |                 | -------- -------- -------                |
        +----------------+-----------------+------------------------------------------+

        - as you can see, each organization gets 2⁸ = 512 usable host addresses from the /23 subnet mask

    • IP addresses are managed under the authority of the Internet Corporation for Assigned Names and Numbers (ICANN).

    • The role of the nonprofit ICANN organization is not only to allocate IP addresses, but also to manage the DNS root servers:

        - the ICANN also allocates addresses to regional Internet registries (for example, ARIN, RIPE, APNIC, and LACNIC, which together form the Address Supporting Organization of ICANN), and handle the allocation/management of addresses within their regions

### Obtaining a Host Address: The Dynamic Host Configuration Protocol

    • Once an organization has obtained a block of addresses, it can assign individual IP addresses to the host and router interfaces in its organization.

        - while a sys/network administrator will typically manually configure IP addresses into the router, host addresses may be configured automatically via DHCP

        - in addition to host IP address assignment, DHCP also allows a host to learn additional information, such as its subnet mask, the address of its first-hop router (often called the default gateway), and the address of its local DNS server

    • Because of DHCP’s ability to automate the network-related aspects of connecting a host into a network, it is often referred to as a plug-and-play or zeroconf (zero-configuration) protocol.

    • DHCP is a client-server protocol: in the simplest case, each subnet (in the addressing sense of Figure 4.20) will have a DHCP server:

        - if no server is present on the subnet, a DHCP relay agent (typically a router) that knows the address of a DHCP server for that network is needed

    • For a newly arriving host, the DHCP protocol is a four step process (as outlined in figure 4.24):

        (1) the client sends out a DHCP discover message within a UPD packet to port 67, which is encapulated within an IP datagram and has the broadcast destination IP address of 255.255.255.255 with a "this host" source address of 0.0.0.0. the link layer then broadcasts this frame to all nodes attached to the subnet

        (2) a DHCP server receiving a DHCP discover message responds to the client with a DHCP offer message that is broadcast to all nodes on the subnet, again using the IP broadcast address of 255.255.255.255.  each server offer message contains the transaction ID of the received discover message, the proposed IP address for the client, the network mask, and an IP address lease time.

        (3) the newly arriving client will choose from among one or more server offers and respond to its selected offer with a DHCP request message, echoing back the configuration parameters

        (4) the server responds to the DCHP request message with a DHCP ACK message, confirming the requested parameters

    • Once the client recieves the DHCP ACK, the interaction is complete and the client can use the DHCP-allocated IP address for the lease duration

## 4.3.3: Network Address Translation (NAT)

    • The address space 10.0.0.0/8 is one of the three portions of the IP address space that is reserved for a private network (these are networks whose addresses only have meaning to devices within that network)

    • Consider the fact that there are hundreds of thousands of home networks, many using the same address space 10.0.0.0/24

        - devices within a given home network can send packets to each other using 10.0.0.0/24 addressing

        - however, packets forwarded beyond the home network into the larger global Internet clearly cannot use these addresses (as either a source or a destination address) because there are hundreds of thousands of networks using this block of addresses

    • In figure 4.25, after understanding the basics of NAT, its important to note that the router gets its address from the ISP’s DHCP server, and the router runs a DHCP server to provide addresses to computers within the NAT-DHCP-router-controlled home network’s address space.

        - since all datagrams arriving at the NAT router from the WAN have the same destination IP address, the router uses a NAT translation table and includes port numbers as well as IP addresses in the table entries to determine to which internal host to forward a given datagram

    • Because of IPv4 address exhaustion, rather than assign public addresses to all of these home NAT routers, your home router itself gets a private IP from the ISP:

        - this is called Carrier-Grade NAT (basically double-layered NAT)

        - ISPs are reserved a special chunck of private IP addresses to do this (100.64.0.0/10)

        - this was done to avoid the case of a first hop home/enterprise NAT and an ISP NAT choosing to use overlapping private IP address spaces

    • If a host inside your network (private IP, e.g., 192.168.1.100) wants to reach another host on the internet, NAT automatically tracks the outgoing connection.

        - but if some external host initiates a connection to a host within your private network, NAT doesn't know which internal device to send the packet to because it wasn't initiated from within your network

        - this means port forwarding is required to manually tell the router "Packets arriving on this public IP + port should go to this internal IP + port"

## 4.3.4: IPV6