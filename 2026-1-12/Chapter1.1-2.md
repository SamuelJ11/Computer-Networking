# Chapter 1.1: What is the Internet

## 1.1.1 A Nuts and Bolts Description

    • Devices that connect to the internet are called "hosts" or "end systems:

        - end systems are connected together by a network of communication links (coax cables, tcp wires, wireless signals, etc) and packet switches

        - different links have different transmission rates measured in bits/second

    • When one end system has data to send to another end system, the sending end system segments the data and adds header bytes to each segment.

        - these resulting packages of information (packets) then arrive at the recieving end system, where they are reassembled into the orignal data

    • A packet switch takes a packet arriving on one of its incoming communication links and forwards that packet on one of its outgoing communication links. 

        - two main types of packet switches are routers and link-layer switches

    • The sequence of communication links and packet switches traversed by a packet from the sending end system to the receiving end system is known as a route or path through the network.

    • End systems access the Internet through Internet Service Providers (ISPs), including residential ISPs such as local cable or telephone companies, corporate ISPs, university ISPs, etc.

        - each ISP itself is a network of packet switches and communication links

    • Lower-tier ISPs are those that provide access to end systems and are thus interconnected through national and international upper-tier ISPs (which are directly connected to each other)

    • End systems, packet switches, and other pieces of the Internet run protocols that control the sending and receiving of information within the Internet

    • Internet standards are developed by the Internet Engineering Task Force (IETF)

        - the IETF standards documents are called Requests For Comments (RFCs) and they define protocols such as TCP, IP, HTTP, etc

## 1.1.2 A Services Description

    • Internet applications are said to be distributed applications since are split across multiple machines that talk to each other but also run on end systems - they do not run in the packet switches in the network core

    • End systems attached to the Internet provide a socket interface that specifies how a program running on one end system asks the Internet infrastructure to deliver data to a specific destination program running on another end system.

## 1.1.3 What is a Protocol

    A. Network Protocols

        • All activity in the Internet that involves two or more communicating remote entities is governed by a protocol

            - for example, hardware-implemented protocols in two physically connected computers control the flow of bits on the "wire" between the two network interface cards
       

## Chapter 1.2: The Network Edge
