# 3.1: Introduction and Transport-Layer Services

    • A transport-layer protocol provides for logical communication between application processes running on different hosts.

    • On the sending side, the transport layer converts the application-layer messages it receives from a sending application process into transport-layer packets, known as transport-layer segments in Internet terminology.

## 3.1.1: Relationship Between Transport and Network Layers

    • A transport-layer protocol provides logical communication between processes running on different hosts, a network-layer protocol provides logical communication between hosts. 

    • Transport-layer protocols live in the end systems. Within an end system, a transport protocol moves messages from application processes to the network edge (that is, the network layer) and vice versa, but it doesn’t have any say about how the messages are moved within the network core.

    • A transport protocol can offer reliable data transfer service to an application even when the underlying network protocol is unreliable, that is, even when the network protocol loses, garbles, or duplicates packets

## 3.1.2: Overview of the Transport Layer in the Internet

    • For this book TCP and UDP packets are reffered to as segments, and the term datagram is reserverd for the network-layer packet.

    • The Internet Protocol is the Internet's network layer protocol

    • The IP service model is a best-effort delivery service. This means that IP makes its “best effort” to deliver segments between communicating hosts, but it makes no guarantees. 

    • Every host has at least one network-layer address, a so-called IP address.

    • The most fundamental responsibility of UDP and TCP is to extend IP’s delivery service between two end systems to a delivery service between two processes running on the end systems:

        - Extending host-to-host delivery to process-to-process delivery is called transport-layer multiplexing and demultiplexing

## 3.2: Multiplexing and Demultiplexing
