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
    
    

