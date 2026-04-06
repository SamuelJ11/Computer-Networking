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