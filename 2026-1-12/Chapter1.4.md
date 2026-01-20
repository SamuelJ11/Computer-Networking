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
    • When characterizing queuing delay, one typically uses statistical measures, such as average queuing delay, variance of queuing delay, and the probability that the queuing delay exceeds some specified value.

    • Let a denote the average rate at which packets arrive at the queue (packet/sec).

    • Recall that R is the transmission rate; that is, it is the rate (in bits/sec) at which bits are pushed out of the queue.

    • Suppose also that all packets consist of L bits

    • Finally, if we assume the queue is very big (i.e. an infinite amount of bits can be stored) 

    • Given the assumption above, we can calculate the traffic intensity (how busy a link (or router) is relative to its capacity) as L.a/R

        - the golden rule is to design your system so that the traffic intensity is no greater than 1

    • If N packets arrive simultaneously (a "burst"), they have to wait in line because the router can only "push" one onto the wire at a time

        - the general formula for the delay of the Nth packet is (n - 1).(L/R)

    • When the traffic intensity is close to 1, there will be intervals of time when the arrival rate exceeds the transmission capacity (due to variations in packet arrival rate), and a queue will form during these periods of time
    
        - when the arrival rate is less than the transmission capacity, the length of the queue will shrink

        - figure 1.19 shows that as the traffic intensity approaches 1, the average queuing delay increases asymptotically

    A. Packet Loss

        • Because the queue capacity is finite, packet delays do not really approach infinity as the traffic intensity approaches 1.

            - instead, a packet can arrive to find a full queue, in which case a router will drop that packet

        • This is why performace at a node is measured not only in terms of delay, but also in terms of the probability of packet loss

## 1.4.3 End to End Delay

    • Assuming there are N - 1 routers between the source host and the destination host, and the network is uncongested (no queuing delay), then we arrive at the total nodal delay (end-to-end delay) with the formula below:

        - dend-to-end = N(dproc + dtrans + dprop)

    A. Traceroute

        • Traceroute is a simple program that can run in any Internet host, whereby the program sends a series of special packets into the network, with each packet addressed to the final destination. 
        
            - the first of these packets is marked , the second is marked , and so on
            
            - whenever a router between the source and destination receives one of these special packets, it decrements the marking by one and then forwards the packet along its way
            
            - however, if at a router the marking gets decremented to zero, the router does not forward the packet toward its destination, but instead sends a message back to the source

            - after receiving the message from the host, the source stops sending these special packets

            * basically, each packet has a hop limit (called the TTL), and each subsequent packet probes one more router along the path, and traceroute sends 3 packets per hop

    B. End System, Application, and Other Delays

        • Media packetization delay is another important delay present in video conferencing, where the sending side must first fill a packet with encoded digitized video and speech before passing the packet to the Internet.

## Throughput in Computer Networks

    • The instantaneous throughput at any instant of time is the rate (in bits/sec) at which the destination host is recieving the file from the source.

    • The average througput is given by the F/T where F = the number of the bits, and T = time in seconds

    • When examining figure 1.20, we see the throughtput is the transmission rate of the bottleneck link, given by min(Rs, Rc)

        - thus the time it takes to transfer a file of size F from server to client is given as F/min(Rs, Rc)

    • Figure 1.21 shows that the constraining factor for throughput in today’s Internet is typically the access network, as the core of the Internet is over-provisioned with high-speed links that experience little congestion