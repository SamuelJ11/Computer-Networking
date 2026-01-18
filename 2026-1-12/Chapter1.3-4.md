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

        • A circuit in a link is implemented with either frequency-division multiplexing (FDM) or time-division multiplexing (TDM)

            - with FDM, the link's frequency spectrum is divided up among it's connections giving a dedicated frequency band to each connection for the duration of the connection

                -- the width of this band is called the bandwidth, and telephone and FM radio stations use FDM to share the frequency spectrum

            - for a TDM link, time is divided into frames of fixed duration, and each frame is divided into a fixed number of time slots

                -- when the network establishes a connection across a link, the network dedicates one time slot in every frame to this connection

                * a frame is just a repeating slice of time:

                  time →  | Frame 1 | Frame 2 | Frame 3 | Frame 4 | ...

                  each frame is the same length and is broken down into smaller pieces called time slots:

                  frame = | Slot 1 | Slot 2 | Slot 3 | Slot 4 |

                  each slot belongs to a different connection:

                  slot 1 → Alice’s phone call

                  slot 2 → Bob’s phone call

                  slot 3 → Netflix stream

                  slot 4 → Empty

                  if a connection is established, the network reserves the same slot in every frame for that connection:

                  frame 1: | A | B | N | - |
                  frame 2: | A | B | N | - |
                  frame 3: | A | B | N | - |
                  frame 4: | A | B | N | - |
                
        • For TDM, the transmission rate of a circuit is equal to the frame rate multiplied by the number of bits in a slot

            - e.g., if a link transmits 8000 frames per second and each slot consists of 8 bits, then the transmission rate of each circuit is (8000 x 8) = 64,000 bps or 40 kbps

            - !!VERY IMPORTANT!! we are talking about the transmission rate of a CIRCUIT, which only has one slot, not the tranmsission rate of the link, which has multiple slots

        • Consider a numerical example of a circuit switched network file transfer:

            - we want to send a file of 640,000 bits from Host A to Host B

            - all links in the network use TDM with 24 slots and have a bit rate of 1.536 Mbps

            - it takes 500 miliseconds to establish an end-to-end circuit before Host A cna begit to transmit the file

                -- since each circuit has a transmission rate of (1.536 Mbps / 24) = 64 kbps, it takes (640,000 bits / 64 kbps) = 10 seconds to transmit the file

                -- this 10 seconds we add to the circuit establishment time, giving 10.5 seconds to send the file

            * note the transmission time is independent of the number of links because each circuit has the same dedicated rate on every link

            * more links DO however increase propagation delay

    B. Packet Switching Versus Circuit Switching

        • Critics of packet switching argue that it is not suitable for real-time services because of variable and unpredictable queuing delays.

        • Proponents argue that it (1) offers better sharaing of transmission capacity and (2) is simpler, more efficient and less costly to implement than circuit switching

        • For example, with packet switching, consider the following:

            - probability that a specific user is active is 0.1

            - there are 35 users, each sharing a link that can support 1 Mbps

                -- the probability that there are 11 or more simultaneously active users is approximately 0.0004 (calculated using the binomial theorem)

        • Consider a second example involving circuit switching where:

            - the link transmission rate is 1,000,000 bps (1 Mbps)

            - there are 10 users

            - each slot is 1000 bits, so first we need to find the time it takes for the link to "sweep" across one slot:

                -- slot sweep time = 1000 bits / 1 Mpbs = 0.001 seconds (1 ms)

            - slots are grouped into frames and one frame contains 10 slots (users):

                -- time per frame = 10 slots x 1ms per slot = 10ms per frame

                -- this means every 10 ms, a new frame starts

            - in TDM circuit switching, the active user sends 1000 bits in slot 1, then must wait for slots 2 - 10 to pass before they can send their next 1000 bits

                -- effectively the user only sends 1000 bits every 10 ms

            - since the user has 1,000,000 bits total to send, we can find how many frames are needed:

                -- number of frames = 1,000,000 / 1000 bits per frame = 1000 frames

            - now we multiply the number of frames by the time each frame takes

                -- total time = 1000 frames x 10ms per frame = 10,000 ms or 10 seconds

        • In the case of packet switching, the active user can continuously send its packets at the full link rate of 1 Mbps, reducing the time taken by an order of magnitude.

        • Although packet switching and circuit switching are both prevalent in today’s telecommunication networks, the trend has certainly been in the direction of packet switching.

## 1.3.3 A Network of Networks




        
