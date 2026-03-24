# 3.7: TCP Congestion Control

## 3.7.1: Classic End-End TCP Congestion Control

    • The approach taken by TCP is to have each sender limit the rate at which it sends traffic into its connection as a function of perceived network congestion

    • There are now many different “flavors” of TCP congestion control that are used in the Internet:

        - the four most popular TCP congestion control algorithms are
  
            (1) “Classic” TCP
            (2) TCP Vegas
            (3) CUBIC TCP
            (4) BBR

    • In Section 3.5, we saw that each side of a TCP connection consists of a receive buffer, a send buffer, and several variables such as LastByteRcvd, LastByteRead, RcvBuffer, etc.

        - there is an additional TCP congestion-control mechanism operating at the sender that keeps track of an additional variable, the congestion window (cwnd)

        - cwnd imposes a constraint on the rate at which a TCP sender can send traffic into the network.

    • Recall the formula for the amount of unacknowledged "in flight" bytes (TCP is a byte stream) is LastByteSent - LastByteAcked;

        - TCP looks at two limits and chooses the strictest one
         
            (1) rwnd is the receiver's limit: the TCP sender won't send more than 'rwnd' bytes to prevent the recievers buffer from overflowing (this is Flow Control from 3.5)
            
            (2) cwnd is the network's limit: the TCP sender won't send more than 'cwnd' bytes so as to not congest the network (this is Congestion Control)
            
        - Analogy: if you are pouring water into a funnel, 'rwnd' is the size of the bottle at the bottom and 'cwnd' is the width of the funnel neck
    
    • Understanding the previous bullet allows us to understand the following equation:

        LastByteSent - LastByteAcked <= min(rwnd, cwnd)

        "the number of in-flight packets must not exceed either the reciever's available buffer space or the network's router buffers (whichever is SMALLEST)

    • In order to focus on congestion control (as opposed to flow control), let’s assume from now on that the TCP receive buffer is so large that the receive-window constraint can be ignored:

        - thus, the amount of unacknowledged data at the sender is solely limited by 'cwnd'
  
    • Since 'cwnd' is the total amount of data you are allowed to have in the network at once, we can roughly estimate the sender's send rate via the equation:

        cwnd (bytes) / RTT (seconds)

        - a critical observation here is that by adjusting the value of 'cwnd', the sender can adjust the rate at which it sends data into its connection

    • Let us define a “loss event” at a TCP sender as the occurrence of either a timeout or the receipt of three duplicate ACKs from the receiver (recall fast retransmit from 3.5.4)

        (1) when there is excessive congestion, then one (or more) router buffers along the path overflows, causing a datagram (containing a TCP segment) to be dropped
         
        (2) the dropped datagram, in turn, results in a loss event at the sender (timeout or 3 duplicate ACKs) which is taken as an indication of congestion on the sender-to-receiver path

        (3) thus in classic TCP, congestion is inferred via a loss event

    • In the more optimistic case when the network is congestion-free, successfully recieved acknowledgments are used to increase its congestion window size (and hence the sender's transmission rate).

        - note that if acknowledgments arrive at a relatively slow rate (end-end path has high delay or contains a low-bandwidth link), then the congestion window will be increased at a relatively slow rate

        - in this way, because TCP uses acknowledgments to trigger (or clock) its increase in congestion window size, TCP is said to be "self-clocking"
  
    • So how then do TCP senders determine their sending rates such that they don’t congest the network but at the same time make use of all the available bandwidth? 

        (1) a lost segment implies congestion, and hence, the TCP sender’s rate should be decreased when a segment is lost

        (2) an acknowledged segment indicates that the network is delivering the sender’s segments to the receiver, and hence, the sender’s rate can be increased 

        (3) the TCP sender thus increases its transmission rate to probe for the rate that at which congestion onset begins, backs off from that rate, and then to begins probing again to see if the congestion onset rate has changed

    • The classic TCP algorithm has three major components: (1) slow start, (2) congestion avoidance, and (3) fast recovery.

### Slow Start

    • When a TCP connection begins, the value of 'cwnd' is typically initialized to a small value of 1 MSS (1460 bytes).

        - this results in the sending rate to be initialized as roughly 1 MSS/RTT
  
    • In Slow Start, the value of 'cwnd' begins at 1 MSS and increases by 1 MSS every time a transmitted segment is first acknowledged.

        - referencing figure 3.49, this process results in a doubling of the sending rate every RTT as each time a segment is acknowledged, 'cwnd' increments by 1 MSS
  
        - the TCP send rate starts slow but grows exponentially fast during the slow-start phase
  
    • If there is a loss event (congestion) indicated by a timeout, the TCP sender sets the value of 'cwnd' to 1 and begins the slow-start process anew.

        - it also sets the value of a second state variable, 'ssthresh' (slow-start threshold) to cwnd / 2 (half of the value of the congestion window when congestion was detected)

        - when the value of 'cwnd' climbs back up to 'ssthresh', slow-start ends and TCP transitions into "congestion avoidance" mode, where TCP increases 'cwnd' more cautiously

### Congestion Avoidance

    • On entry to the congestion-avoidance state, the value of 'cwnd' is approximately half its value when congestion was last encountered.

        - in this mode, TCP increases the value of 'cwnd' by just a single MSS every RTT
  
        * recall that the sending rate is defined to be (N . MSS) / RTT at any given point, where 'N' is the number of segments
        * for every RTT that passes without a loss, N becomes N + 1
  
    • A common approach is for the TCP sender to increase 'cwnd' by MSS/cwnd bytes whenever a new acknowledgment arrives.
    
    • Consider this example to make the previous point more concrete:

        - lets say 'MSS' is defined to be 1460 bytes and 'cwnd' is 14600 bytes

        * pause here: # of segments in flight = cwnd / MSS
  
        - this means 10 segments are being sent within an RTT, and each arriving ACK (assuming one ACK per segment) increases the congestion window size by 1/10 MSS
     
        - thus, the value of the congestion window will have increased by one MSS only when ALL 10 ACKs segments have been received
  
    • When a timeout occurs during congestion avoidance, TCP's congestion-avoidance algorithm behaves the same as in the case of slow-start:

        - the value of 'cwnd' is set to 1 MSS, and the value of 'ssthresh' is updated to half the value of 'cwnd' when the loss event occured

    • If loss is inferred by a triple duplicate ACK event, TCP (1) halves the value of 'cwnd' (adding in 3 MSS to account for the triple duplicate ACKs) and (2) sets 'ssthresh' to be half of 'cwnd' when the triple duplicate ACKs were recieved.

    • One of the earliest versions of TCP, known as "TCP Tahoe", always cut its congestion window to 1 MSS and entered the slow-start phase after either a timeout-indicated or triple-duplicate-ACK-indicated loss event. 

    • The next version of TCP congestion control, "TCP Reno", incorporated the change of responding less drastically to congestion indicated by a triple duplicate ACK by halving the value of 'cwnd' and adding three.

        * see figure 3.50 for an illustration of TCP Tahoe vs Reno

### TCP Congestion Control: Retrospective

    • Ignoring the initial slow-start period when a connection begins and assuming that losses are indicated by triple duplicate ACKs rather than timeouts, we can observe the following:
    
        - TCP’s congestion control consists of linear (additive) increase in 'cwnd' of 1 MSS per RTT and then a halving (multiplicative decrease) of 'cwnd' on a triple duplicate-ACK event

        - For this reason, TCP congestion control is often referred to as an additive-increase, multiplicative-decrease (AIMD) form of congestion control

### Macroscopic Description of TCP Reno Throughput

    • When the congestion window size is 'w' bytes and the current round-trip time is RTT seconds, then TCP's transmission rate is roughly w/RTT/

        - here we denote 'W' by the value of 'w' when a loss event occurs

    • Assuming that RTT and 'W' are approximately constant over the duration of the connection, the TCP transmission rate ranges from W/2RTT to W/RTT
          
        - because TCP’s throughput (that is, rate) increases linearly between the two extreme values, we have
  
        average TCP connection throughput = 0.75W/RTT

## 3.7.2: Recent End-End TCP Congestion Control Algorithms

    • The “classic” TCP congestion control algorithms that we studied in the last section have largely been replaced by the newer flavors of TCP congestion control (in particular, BBR, and before that TCP CUBIC)

### TCP CUBIC: improving on additive increase

    • TCP CUBIC differs only slightly from TCP Reno:
    
        - once again, the congestion window is increased only on ACK receipt, but CUBIC changes the congestion avoidance phase, as follows:

        (1) let 'Wₘₐₓ' be the size of TCP’s congestion control window when loss was last detected, and let 'K' be the future point in time when TCP CUBIC's window size will again reach Wₘₐₓ

        (2) CUBIC TCP increases the congestion window as a function of the cube of the distance between the current time 't' and 'K'; when 't' is further away from 'K', the congestion window size increases are much larger than when it's close to 'K'

        (3) when 't' exceeds 'K', the congestion window increases rapidly to allow CUBIC to more quickly find a new operating point if the congestion level of the link that caused loss has changed significantly

        * see figure 3.52 to see how CUBIC enjoys a higher overall throughput than Reno

### BBR: Bottleneck Bandwidth and Round-trip propagation time

    • The BBR (Bottleneck Bandwidth and RTT) congestion-control protocol is based on the insight of "keeping the pipe full, but not fuller".

        - let Nₚᵢₚₑ be the number of packets "in the pipe" or in flight

        - if our goal is to "keep the pipe just full" then its important to realize that a longer RTT pipe is able to hold more packets

        - as the path’s link capacities increase, it can hold more in-flight packets; we think of "long, fat pipes" as connections with large-bandwidth links and a large RTTs

        - this gives rise to the notion of the "bandwidth-delay product" as an approximation for the average amount of in-flight data that connections can support without loss
  
    • Think of the bandwidth-delay product as the exact moment the pipe is 100% full; with this in mind we reference figure 3.53:

        (1) initially, when 'Nₚᵢₚₑ' increases, the connections’ throughput also increases

        (2) when the amount of in-flight data reaches the connection's bandwidth-delay product, the pipe is “just full enough.” 

        (3) increasing 'Nₚᵢₚₑ' beyond the bandwidth-delay product not only won’t increase throughput, but actaully increases RTT due to the increased queueing delay at the bottleneck link

    • Given these considerations, the point at which 'Nₚᵢₚₑ' equals the bandwidth-delay product can be considered an “ideal” operating point.

## 3.7.3: Network-Assisted Explicit Congestion Notification
    
    • The previous approaches took an end-to-end approach towards congestion control in that they used observations at the TCP sender of packet loss, RTT, and achieved throughput to perform congestion control.

    • Extensions to both IP and TCP [RFC 3168] have been proposed, implemented, and deployed, collectively known as "Explicit Congestion Notification (ECN)", that allow the network to explicitly signal congestion to a TCP sender and receiver.

        - in the IP datagram header, two bits (for which there are two settings) in the Type of Service field are used for this:
  
        (1) one setting is used by a router to indicate that it is experiencing congestion, and this bit can be set before loss actually occurs

        (2) a second setting is used by the sending host to inform routers that the sender and reciever are ECN-capable

        * see figure 3.54 for a clearer example of this
  
## 3.7.4: Fairness

    • Consider 'K' TCP connections, each with a different end-to-end path and taking a loss-based AIMD approach toward congestion control.

        - each of these connections pass through a bottleneck link with transmission rate R bytes/sec

    • A congestion-control mechanism is said to be "fair" if the average transmission rate of each connection is approximately R/K.

    • When referencing figure 3.54, ignore the slow-start phase of TCP and assume the TCP connections are operating in CA mode (AIMD) at all times.

        - looking at figure 3.55 now, the goal is to have the achieved throughputs fall somewhere near the intersection of the equal capacity share line and the full utilization line
  
    • In the previous idealized scenario, we assumed that only TCP connections traverse the bottleneck link, that the connections have the same RTT value, and that only a single TCP connection is associated with a host-destination pair.

        - in reality, when multiple connections share a common bottleneck, those sessions with a smaller RTT are able to grab the available capacity at that link more quickly as it becomes free

### Fairness and UDP

    • When running over UDP, applications can pump their audio and video into the network at a constant rate and occasionally lose packets, rather than reduce their rates to “fair” levels at times of congestion and not lose any packets.

    • Because TCP congestion control will decrease its transmission rate in the face of increasing congestion (loss), while UDP sources need not, it is possible for UDP sources to crowd out TCP traffic.

### Fairness and Parallel TCP Connections

    • Even if we could force UDP traffic to behave fairly, the fairness problem would still not be completely solved:

        - nothings stops TCP-based applications from using multiple parallel connections

    • Web browsers often use multiple parallel TCP connections to transfer the multiple objects within a Web page.