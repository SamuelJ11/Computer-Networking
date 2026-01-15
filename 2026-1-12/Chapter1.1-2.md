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
       

# Chapter 1.2: The Network Edge

    • End systems are also referred to as hosts because they host (that is, run) application programs such as a Web browser or server program, an e-mail client program or an e-mail server program

        - hosts are also further divided into two categories: clients and servers

## 1.2.1 Access Networks

    • The access network is the network that physically connects an end system to the first router (aka the edge router) on a path from the end system ot any other distant end system

    A. Home Access: DSL, Cable, FTTH, Fixed Wireless, and LEO Satellites

        • Today, the three most prevalent types of broadband residential access are (1) digital subscriber line (DSL), (2) cable and (3) fiber to the home (FTTH)

            - With DSL, a customer's DSL modem converts the digital signals from a computer to high frequency analog tones for transmission over telephone wires to the telco's central office

            - at the central office, the analog signals are translated back into digital format at the digital subscriber line access multiplexer (DSLAM) and sent to the ISP's network (here the telco IS the ISP)

        • On the customer side, a splitter seperates the data and telephone signals arriving to the home and forwards the data and phone signals and sends the data into the Internet

        • Generally if the residence is not located within 5 to 10 miles of the CO, the residence must resort to an alternative form of Internet access

        • Cable Internet access makes use of the cable television company's existing cable television infrastructure

            - fiber optics connect the cable head end (in the ISP) to neighborhood-level junctions (fiber nodes) from which traditional coaxial cable is then used to reach individual homes and apartments

            - because both fiber and coaxial cable are employed in this system, it is often reffered to as hybrid fiber coax (HFC)

        • As with a DSL modem, the cable modem termination system (CMTS) in the ISP converts the analog signal sent from the cable modems in many downstream homes back into digital format

            - cable modems (like DSL modems) divide the HFC netowrk into two channels, a downstream and upstream channel, with asymmetric access speeds (downstream is usually faster)

        • FTTH is the faster network technology currently available for residential broadband access

            - there are two competing optical-distrubution network architecutures that perfom the splitting of fiber leaving the central office into individual customer-specific fibers:

                -- active optical networks (AONs)

                -- passive optical networks (PONs)

            - see figure 1.7 for an overview of how PONs work

        • Fixed wireless Internet (FWI) has also become a popular Internet access technology in which data is sent wirelessly from a provider's base station to the modem in the home

        • Low-earth-orbit (LEO) satellites are increasingly being used for broadband Internet access for rural and remote areas (SpaceX's Starlink)

    B. Ethernet and WiFI

        • Ethernet is by far the most prevalent LAN access technology in corportate, university and home networks

        • Increasingly, however, people are accessing the Internet wirelessly from laptops, smartphones, tablets, and other “things” using wireless access points (which in turn are connected to the wired internet)

    C. Wide-Area Wireless Acess: 4G and 5G

        • Mobile devices employ the same wireless infrastructure used for cellular telephony to send/receive packets through a base station that is operated by the cellular network provider.

## 1.2.2 Physical Media

    • Physical media fall into two categories:

        - guided media: the waves are guided along a solid medium, such as a fiber-optic or coax cable

        - unguided media: the waves propogate in the atmosphere and in outerspace

        A. Twisted-Pair Copper Wire

            • More than 99 percent of the wired connections from the telephone handset ot the local telephone switch use twisted-pair copper wire

            • Consists of two insulated copper wires arranged in a regular spiral pattern (they are twisted together to reduce the electrical interference from similar pairs close by)

                - typically, a number of pairs are bundled together in a cable by wrapping them in a protective shield (UTP is commonly used for networks within a building)

        B. Coaxial Cable

            • Like twisted pair, coaxial cable consists of two copper conductors, but the two conductors are concentric rather than parallel.

            • Coaxial cable is quite common in cable television systems and cable Internet access, where the transmitter shifts the digital signal to a specific analog frequency band, which is then sent from the transmitter to one or more recievers.

                - can be used as a guided shared medium

        C. Fiber Optics

            • An optical fiber is a thin, flexible medium that conducts pulses of light, with each pulse representing a bit

                - immune to electromagnetic interference, have very low signal attenuation up to 100 kilometers, and are very hard to tap

            • The Optical Carrier (OC) standard link speeds range from 51.8 Mbps to 39.8 Gbps these specifications are often referred to as n x 51.8 Mbps:

                - ex. OC-1, OC-3, OC-12, OC-24, OC-48, OC-96, OC-192, OC-768

        D. Terrestrial Radio Channels

            • Can be broadly classified into three groups:

                (1) those that operate over very short distance (1-2 meters)
                (2) those that operate in local areas (10 to a few hundred meters)
                (3) those that operate in the wide area, spanning tens of kilometers

        E. Satellite Radio Channels

            • A communication satellite links two or more Earth-based microwave transmitter/receivers, known as ground stations. 
            
                - the satellite receives transmissions on one frequency band, regenerates the signal using a repeater, and transmits the signal on another frequency

            • Two types of satellites are used in communications: 
            
                - geostationary satellites permanently remain above the same spot on Earth, and are the central component of the global positioning system (GPS)

                - LEO satellites are placed much closer to Earth and do not remain permanently above one spot on Earth are increasingly used for Internet access particularly in remote areas 



    