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