# Chapter 1.4: Delay, Loss, and Throughput in Packet-Switched Networks

## 1.4.1 Overview of Delay in Packet-Switched Networks

    • As a packet travels from one node (host or router) to the subsequent node along a path, the packet suffers from several types of delays.

    • Breifly recall that a packet can be transmitted on a link only if there is no other packet currently being transmitted on the link and if there are no other packets preceding it in the queue.

    A. Types of Delay

        • There are four main types of delay affecting the performance of Internet applications:

            (1) processing delay - the time required to examine the packet's header and determine where to direct the packet, typically on the order of microsends or less in high-speed routers

            (2) queuing delay - the packet is waits to be transmitted onto the link, and the number of packets that an arriving packet might expect to find in the queue is a function of the intensity and nature of the traffic arriving at the queue

            (3) transmission delay - recall the transmission delays is given by L (length of packet) / R (transmission rate of the link), and represents the amount of time required to push (that is, transmit) all of the packet’s bits into the link

            (4) propogation delay - the time required to propogate from the beginning of the link to the next router, dependent on the physcial medium of the link

                - calculated via d (the distance between two routers) / s (the propagation speed)

    B. Comparing Transmission and Propagation Delay

        • The transmission delay has nothing to do with the distance between the two routers, and the propagation delay has nothing to do with the packet's length or the transmission rate of the link

        • The total nodal delay is given by the sum of the processing, queuing, transmission and propagation delays

## 1.4.2 Queuing Delay and Packet Loss
	​
    •


