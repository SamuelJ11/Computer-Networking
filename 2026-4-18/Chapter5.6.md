# 5.6: ICMP: The Internet Control Message Protocol

    • The Internet Control Message Protocol (ICMP) is used by hosts and routers to communicate network-layer information to each other.

    • ICMP is often considered part of IP, but architecturally it lies just above IP, as ICMP messages are carried inside IP datagrams.

    • ICMP messages have a type and a code field, and contain the header and the first 8 bytes of the IP datagram that caused the ICMP message to be generated in the first place

        * when a router drops your packet and sends an ICMP error back to you, it copies the IP header and the first 8 bytes of data from your original packet and glues them into the ICMP message

        * recall the TCP and UPD segment/datagram header structures plust the IP datagram header structure:

         _____________________32 bits__________________
        /                                              \
        +-----------------------+-----------------------+
        |    Source port #      |     Dest. port #      |
        +-----------------------+-----------------------+
        |        Length         |       Checksum        |
        +-----------------------+-----------------------+
        |                                               |
        |               Application data                |
        |                  (message)                    |
        |                                               |
        +-----------------------------------------------+

         ___________________________32 bits____________________________
        /                                                              \
        +-------------------------------+-------------------------------+
        |         Source port #         |          Dest. port #         |
        +-------------------------------+-------------------------------+
        |                        Sequence number                        |
        +---------------------------------------------------------------+
        |                      Acknowledgment number                    |
        +-------+-------+---------------+-------------------------------+
        |Header |       |C|E|U|A|P|R|S|F|                               |
        |length |Unused |W|C|R|C|S|S|Y|I|         Receive window        |
        |       |       |R|E|G|K|H|T|N|N|                               |
        +-------+-------+---------------+-------------------------------+
        |       Internet checksum       |      Urgent data pointer      |
        +-------------------------------+-------------------------------+
        |                                                               |
        |                            Options                            |
        |                                                               |
        +---------------------------------------------------------------+
        |                                                               |
        |                             Data                              |
        |                                                               |
        +---------------------------------------------------------------+

         ____________________________32 bits___________________________
        /                                                              \
        +-------+-------+---------------+-------------------------------+
        |Version|Header |Type of service|    Datagram length (bytes)    |
        |       |length |               |                               |
        +-------+-------+---------------+-------+-----------------------+
        |        16-bit Identifier      | Flags |13-bit Fragmentation   |
        |                               |       |        offset         |
        +---------------+---------------+-------+-----------------------+
        |  Time-to-live |  Upper-layer  |        Header checksum        |
        |               |   protocol    |                               |
        +---------------+---------------+-------------------------------+
        |                      32-bit Source IP address                 |
        +---------------------------------------------------------------+
        |                   32-bit Destination IP address               |
        +---------------------------------------------------------------+
        |                        Options (if any)                       |
        +---------------------------------------------------------------+
        |                                                               |
        |                             Data                              |
        |                                                               |
        +---------------------------------------------------------------+

        * this means for an ICMP message, it contains the IP header of the IP datagram that it is itself contained in, and (1) if the packet is a UDP datagram, the first 8-bytes are the entire header itself, and (2) if the packet is a TCP segment, then the first 8-bytes contain the source + destination port numbers as well as the sequence number

    • Note than an ICMP error packet actually contains two different IP headers, and it's easy to mix them up:

        - the outer IP header is the datagram the ICMP message is itself contained in; the source IP is the router that dropped your packet, and the destination IP is your computer

        - the inner IP header is the original, broken envelope that the router copied; the source IP is your computer, and the destination IP is the server (or other host) you were trying to reach

    • Below are some common ICMP message types (copied from figure 5.19):

        +-----------+------+--------------------------------------+
        | ICMP Type | Code | Description                          |
        +-----------+------+--------------------------------------+
        |     0     |   0  | echo reply (to ping)                 |
        |     3     |   0  | destination network unreachable      |
        |     3     |   1  | destination host unreachable         |
        |     3     |   2  | destination protocol unreachable     |
        |     3     |   3  | destination port unreachable         |
        |     3     |   6  | destination network unknown          |
        |     3     |   7  | destination host unknown             |
        |     4     |   0  | source quench (congestion control)   |
        |     8     |   0  | echo request                         |
        |     9     |   0  | router advertisement                 |
        |    10     |   0  | router discovery                     |
        |    11     |   0  | TTL expired                          |
        |    12     |   0  | IP header bad                        |
        +-----------+------+--------------------------------------+

        * the well-known ping program sends an ICMP type 8 code 0 message to the specified host
        * the destination host, seeing the echo request, sends back a type 0 code 0 ICMP echo reply