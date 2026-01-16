# Chapter 1.3: The Network Core

    • The network core is the mesh of packet switches and links that interconnects the Internet’s end systems.

## 1.3.1 Packet Switching

    • To send a message from a source end system to a destination end system, the source breaks long messages into smaller chunks of data known as packets

    • Each packet travels through communication links and packet switches (for which there are two types: routers and link-layer switches)

    • If a source end system or a packet switch is sending a packet of L bits of a link (direct physical or wireless connection between two network devices) with transmission rate R bits/sec, then the time to transmit the packet is L/R seconds

    A. Store and Forward Transmission

        • Most packet switches use store-and-forward transmission at the inputs to the links

            - this means that the packet switch must recieve the entire packet before it can begin to transmit the first bit of hte packet onto the outbound link

            - only after the router has received all of the packet’s bits can it begin to transmit (forward) the packet onto the outbound link

                -- thus at 2L/R seconds, the router has transmitted the entire packet and it has been recieved by the destination for figure 1.11

        • For the general case where we have N links each of rate R (thus there are N-1 routers) we can apply the same logic and conclude that the end-to-end delay is N(L/R)

            - additionally we conclude that the delay for P packets sent over a series of N links is given by (P + N - 1)(L/R)

    B. Queuing Delays and Packet Loss

        • For each attached link, the packet switch has an output buffer (also called an output queue) which stores packets that the router is about to send into that link

        • Packets thus also suffer queueing delays which are variable and depend on the level of congestion in the network

            - if an arriving packet finds that the buffer is completely full with other packets waiting for transmission, packet loss will occur

            - see figure 1.12 to get a visual of packet congestion

    C. Forwarding Tables and Routing Protocols

        • In the Internet, every end system has an address called an IP address.

            - when a source end system wants to send a packet to a destination end system, the source includes the destination's IP address in the packet's header

            - when a packet arrives at a router, the router examines a portion of the packet's destination address and forwards the packet to an adjacent router

        • Each router has a forwarding table that maps destination addresses to that router's outbound links

            - the router examines the address and searches its forwarding table to find the appropriate outbound link

        • the Internet has a number of special routing protocols tha tare used to automatically set the forwarding tables

## 1.3.2 Circuit Switching

    • In cicruit-switched networks, the resources needed along a path (buffers, link transmission rate, etc) to provide for communication between the end systems are reserved for the duration of the communication session between the end systems.

        - in packet-switched networks, these resources are not reserved

    • Telephone networks are examples of circuilt-switched networks as the switches on the path between the sender and receiver maintain connection state (called a circuit) for that connection

        - when the network establishes the circuit, a given transmission rate has been reserved for this sender-to-reciever connection at a guaranteed constant rate

    A. Multiplexing in Circuit-Switched Networks

        • 


        
