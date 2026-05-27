# 6.7: Retrospective: A day in the Life of a Web Page Request

    • Here we wrap up our journey down the protocol stack by taking an integrated, holistic view of the protocols we’ve learned about so far. 

## 6.7.1: Getting Started: DHCP, UDP, IP, and Ethernet

    • Suppose you are on campus and you connect an ethernet cable to the CS department's switch, which is connected to a router which is connected to the ISP.

        - here we assume the ISP is Comcast, and the DNS server resides in the ISP's network

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

    • 