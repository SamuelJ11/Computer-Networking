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
