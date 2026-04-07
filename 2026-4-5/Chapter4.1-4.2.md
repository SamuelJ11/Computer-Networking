# 4.1: Overview of Network Layer

    • The primary data-plane role of each router is to forward datagrams from its input links to its output links.
    
    • The primary role of the network control plane is to coordinate these local, per-router forwarding actions so that datagrams are ultimately transferred between source and destination hosts. 

## 4.1.1: Forwarding and Routing: The Data and Control Planes

    • The two important network-layer functions are forwarding and routing:

        (1) forwarding refers to the router-local action of transferring a packet from an input link interface to the appropriate output link interface

        (2) routing refers to the network-wide process that determines the end-to-end paths that packets take from source to destination

            - the algorithms that calculate these paths are known as routing algorithms

    • A key element in every network router is its forwarding table:

        - a router forwards a packet by examining the value of one or more fields in the arriving packet’s header, and then using these header values to index into its forwarding table.

        - this is the key function performed by the data-plane functionality of the network layer
  
### Control Plane: The Traditional Approach

    • The routing algorithm determines the content's of the router's forwarding tables.

    • The traditional approach involves having each router having a routing component that communicates with the routing component of other routers.

    • An alternative is having a remote controller that computes and distributes the forwarding tables to be used by each and every router within a network.

        - here the control-plane routing functionality is separated from the physical router
  
    • The control-plane approach shown in Figure 4.3 is at the heart of software-defined networking (SDN).

## 4.1.2: Network Service Model

    • The network service model defines the characteristics of end-to-end delivery of packets between sending and receiving hosts:

        - guaranteed delivery: this service guarantees that a packet sent by a source host will eventually arrive at the destination host

        - guaranteed delivery with bounded delay: this service not only guarantees delivery of the packet, but delivery within a specified host-to-host delay bound

        - in-order packet delivery: this service guarantees that packets arrive at the destination in the order that they were sent

        - guaranteed minimal bandwidth: this network-layer service emulates the behavior of a transmission link of a specified bit rate between sending and receiving hosts

        - security: the network layer could encrypt all datagrams at the source and decrypt them at the destination, thereby providing confidentiality to all transport-layer segments

    • The Internet’s network layer provides a single service, known as best-effort service.

### Chapter 4 Overview

    • To clear up some terminology moving forward:

        - packet switching is the overall method of moving data through a network
       
        - routing is deciding which path the data should take
       
        - forwarding / switching is actually moving the packet to the next hop'

        - network-layer packets are called datagrams and link-layer packets are called frames

# 4.2: What's Inside a Router?

    • The actual transfer of packets from a router’s incoming links to the appropriate outgoing links at that router is illustrated in figure 4.4:

        - regarding the input ports, control packets are forwarded to the routing processor

        - be careful with the term "port" here:  these are physical input and output router interfaces, distinct from the logical ports associated with network appications and sockets
  
    • The switching fabric connects the router’s input ports to its output ports.

    • An output port stores packets received from the switching fabric and transmits these packets on the outgoing link by performing the necessary link-layer and physical-layer functions.

    • In traditional routers, the routing processor executes the routing protocols maintains routing tables and attached link state information, and computes the forwarding table for the router. 
    
        - in SDN routers, the routing processor is responsible for communicating with the remote controller in order to (among other activities) receive forwarding table entries computed by the remote controller, and install these entries in the router’s input ports
  
    • While the data plane operates at the nanosecond time scale, a router’s control functions operate at the millisecond or second timescale.

## 4.2.1: Input Port Processing and Destination-Based Forwarding

    • Referencing figure 4.5, the input port has three main functions/blocks:

        (1) line termination: handles the electrical/optical signal coming in (conversts bits on the wire to usable data)

        (2) link-layer processing: handles frame parsing, error detection and removing the link-layer header (converts link-layer frames to network layer datagrams)

        (3) lookup + forwarding: examines the IP header and does a forwarding table lookup (decides which output port to send the packet to)

    • A line card is a full hardware board (like a circuit board) inside the router, and contains:

        (1) multiple ports

        (2) input port logic

        (3) memory for forwarding table copies

        (4) hardware for fast packet processing

    • The routing processor computes the forwarding table, sends it over PCI bus to the line cards which then keep their own local forwarding table copies

    • With such a shadow copy at each line card, forwarding decisions can be made locally, at each input port, without invoking the centralized routing processor on a per-packet basis and thus avoiding a centralized processing bottleneck.

    • In SDN, the router’s routing processor is either minimized or doesn’t run routing protocols—the SDN controller computes the forwarding table and installs it into the router.

        - router processor simply talks to the remote controller, clear separation between control and data plane
  
    • Let's now consider how incoming packets are switched to thier corresponding output ports:

        - suppose our router has four links, numbered 0 - 3, and that packets are to be forwarded to the link interface as follows:

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

        - with this style of forwarding table, the router matches a prefix of the packet’s destination address with the entries in the table; if there’s a match, the router forwards the packet to a link associated with the match

        - e.g., if the packets destination address is 11001000 00010111 00010110 10100001, then router forwards teh packet to link interface 0

    • When there are multiple matches, the router uses the longest prefix matching rule.

    • At Gigabit transmission rates, this lookup must be performed in nanoseconds.
    
        - in practice, Ternary Content Addressable Memories (TCAMs) are often used for address lookup
  
        - since TCAMs are expensive and have high energy consumption, hybrid approaches that combine static random-access memory (SRAM) memories have also been proposed, along with purely “algorithmic” approaches to lookup

## 4.2.2: Switching

    • The switching fabric is at the very heart of a router, as it is through this fabric that the packets are actually switched (that is, forwarded) from an input port to an output port.

    • Switching can be accomplished in a number of ways (see figure 4.6):

        (1) switching via memory: in traditional routers, the packed was copied from the input port into processor memory, then the routing processor then extracted the destination address from the header, looked up the appropriate output port in the forwarding table, and copied the packet to the output port’s buffers. modern routers that use this method have the memory copying and forward table lookup done via the input port line cards

        (2) switching via a bus: this is done by having the input port pre-pend a switch-internal label (header) to the packet indicating the local output port to which this packet is being transferred and transmitting the packet onto the bus. all output ports receive the packet, but only the port that matches the label will keep the packet. the label is then removed at the output port.  because every packet must cross the single bus, the switching speed of the router is limited to the bus speed

        (3) switching via an interconnection network: one way to overcome the bandwidth limitation of the previous methods is to use a crossbar switch, an interconnection network consisting of 2N buses that connect N input ports to N output ports.  unlike the previous two switching approaches, crossbar switches are capable of forwarding multiple packets in parallel. a crossbar switch is non-blocking — a packet being forwarded to an output port will not be blocked from reaching that output port as long as no other packet is currently being forwarded to that output port

## 4.2.3: Output Port Processing

    • This involves taking packets that have been stored in the output port’s memory and transmitting them over the output link (see figure 4.7).

## 4.2.4: Where Does Queuing Occur?

    • Recall that in our earlier discussions, we said that packets were “lost within the network” or “dropped at a router.”
    
        - it is here, at these input and output queues within a router, where such packets are actually dropped and lost

    • If the switching fabric is N times faster than the input/output line rate, it can move packets from inputs to outputs faster than they arrive, preventing significant queuing at the input ports. Even in the worst-case scenario where all input packets target the same output, the fabric can clear one full batch of packets before the next batch arrives.

### Input Queuing

    • See figure 4.8 for an illustration of Head of Line Blocking in an input-queued switch.

### Output Queueing

    • Imagine a switch with 4 input ports and 1 output port (N = 4):
    
        (1) arrival: 4 packets arrive simultaneously, one at each input port.
        
        (2) internal transfer: because the switch is 4 times faster than the lines, it can grab all 4 of those packets and shove them toward the single output port in the time it takes the output port to send just one packet
        
        (3) the accumulation: while the output port is busy transmitting packet #1 onto the wire, packets #2, #3, and #4 have already arrived and are sitting there waiting  
        
        (4) the cycle continues: by the time the output port finishes packet #1 and starts packet #2, 4 more packets might have arrived from the inputs.
        
        (5) the result: packet loss because packets are arriving at the output port faster than they can be sent out, meaning they have to stay in a buffer (memory) which eventually overflows

### How Much Buffering Is “Enough?”

    • For many years, the rule of thumb for buffer sizing was that the amount of buffering 'B' should be equal to an average round-trip time 'R' (in ms) times the link capacity 'C' (in bits/s) such that B = R * C

    • More recent theoretical and experimental efforts suggest that when a large number of independent TCP flows N pass through a link, this equation gets updated to B = R * (C / sqrt(N))

        - this is seen in core networks with many TCP flows
  
    • To understand "bufferbloat" from figure 4.10, consider this scenario:

        Phase 1: The Initial Burst (t = 0)
        - burst: the gamer’s PC sends 25 packets to the home router all at once.
        - router queue: 25 packets now sitting in the buffer.
        - bottleneck: the internet connection can only send 1 packet every 20 ms.

        Phase 2: The "Silent" Period (t = 0 to t = 200 ms)
        - first packet in transit: the very first packet is traveling to the server; no ACKs have returned yet.
        - router draining: the router starts transmitting packets one by one.
        - timeline:
            - t = 0 ms → Packet 1 starts sending.
            - t = 20 ms → Packet 2 starts sending.
            - t = 40 ms → Packet 3 starts sending.
            - t = 60 ms → Packet 4 starts sending.
            - t = 80 ms → Packet 5 starts sending.
            - t = 100 ms → Packet 6 starts sending.
            - t = 120 ms → Packet 7 starts sending.
            - t = 140 ms → Packet 8 starts sending.
            - t = 160 ms → Packet 9 starts sending.
            - t = 180 ms → Packet 10 starts sending.
            - t = 200 ms → 10 packets have been sent; the router is starting Packet 11.

        Phase 3: The ACK Arrival (t = 200 ms)
        - RTT: the first packet sent at t = 0 finally reaches the server, and its ACK arrives back at the PC.
        - ACK clocking: the PC sees the ACK for Packet 1 and thinks there’s space to send the next packet.
        - new packet sent: the PC sends Packet 26 to the router.

        Phase 4: The Persistent Queue
        - queue state: original packets remaining = 25 − 10 = 15 (Packets 11–25).
        - new arrivals: for every 20 ms, one ACK arrives, prompting the PC to send one new packet.
        - packets leaving: router sends one packet every 20 ms.
        - balance: one in = one out. The queue stays at 15 packets indefinitely.
        - consequence: even though the router is sending at full line speed, it can never clear the initial burst because new packets arrive at the exact same rate it drains the queue.

    • The above scenario illustrates that not only is throughput important, but also minimal delay is important as well.

## 4.2.5: Packet Scheduling

### First-in-First-Out (FIFO)

    • The FIFO (also known as first-come-first-served, or FCFS) scheduling discipline selects packets for link transmission in the same order in which they arrived at the output link queue.

        - see figure 4.12 for the FIFO queue in operation

### Priority Queuing
  
    • Under priority queuing, packets arriving at the output link are classified into priority classes upon arrival at the queue (see figure 4.13).

        - the choice among packets in the same priority class is typically done in a FIFO manner

        - - see figure 4.14 for the priority queue in operation

### Round Robin and Weighted Fair Queuing (WFQ)

    • Under the round robin queuing discipline, packets are sorted into classes as with priority queuing. 
    
        - however, rather than there being a strict service priority among classes, a round robin scheduler alternates service among the classes

    • A work-conserving round robin discipline that looks for a packet of a given class but finds none will immediately check the next class in the round robin sequence (see figure 4.15).

    • A generalized form of round robin queuing that has been widely implemented in routers is the so-called weighted fair queuing (WFQ) discipline (see figure 4.16) and the paragraph that follows, its pretty straightforward.