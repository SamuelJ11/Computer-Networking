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