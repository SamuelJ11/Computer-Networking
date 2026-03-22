# 3.6: Principles of Congestion Control

    • Packet retransmission treats a symptom of network congestion (the loss of a specific transport-layer segment) but does not treat the cause of network congestion:
    
        - to treat the cause of network congestion, mechanisms are needed to throttle senders in the face of network congestion

## 3.6.1: The Causes and the Costs of Congestion

    • Here we’ll not (yet) focus on how to react to, or avoid, congestion but rather focus on the simpler issue of understanding what happens as hosts increase their transmission rate and the network becomes congested.

### Scenario 1: Two Senders, a Router with Infinite Buffers

    • Referencing figure 3.41, we assume that Host A is sending data into the connection at an average rate of λᵢₙ bytes/sec.

        - we assume the underlying transport-level protocol is simple - no error recovery (retransmission), flow control, or congestion control is performed

    • Thus the rate at which Host A offers traffic to the router is thus λᵢₙ bytes/sec; Host B operates in a similar manner.

    • Referencing figure 3.42, these two graphs show what happens to your connection as you try to send more and more data λᵢₙ into a shared pipe.

        - graph A shows how much data actually makes it to the receiver (throughput = λₒᵤₜ); λₒᵤₜ can never exceed R/2 because the the link capacity 'R' is shared by Host A and Host B.

        - for a sending rate between 0 and R/2, the througput at the receiver equals the sender's sending rate

        - achieving a per-connection throughput of R/2 might appear to be a good thing, because the link is fully utilized, but graph B shows the consequence of operating near link capacity
  
        - as the sending rate gets closer to R/2, the router's buffer starts to fill up and the average delay becomes larger and larger; when the sending rate exceeds R/2, the average number of queued packets in the router becomes unbounded and the average delay becomes infinite

### Scenario 2: Two Senders and a Router with Finite Buffers

    • A consequence of finite router buffers is that packets will be dropped when arriving to an already-full buffer. 

    • Here we also assume that if a packet containing a transport-level segment is dropped at the router, the sender will eventually retransmit it

        - because packets can be retransmitted, we must now be more careful with our use of the term "sending rate"

    • Now we define λᵢₙ to be the rate at which the application sends original data into the socket.
    
        - λ'ᵢₙ is thus defined to be the rate at which the transport layer sends segments (containing original data AND retransmitted data) into the network 
  
        - λ'ᵢₙ is sometimes referred to as the offered load to the network
  
    • Consider an unrealistic scenario where Host A is able to somehow determine whether or not a buffer is free in the router and thus sends a packet only when a buffer is free.

        - in this case, no loss would occur and λᵢₙ would equal λ'ᵢₙ and the throughput of the connection would be λᵢₙ
  
    • Next, consider next the slightly more realistic case that the sender retransmits only when a packet is known for certain to be lost.

        - figure 3.44b shows what happens when λ'ᵢₙ equals R/2; at this value, athe rate at which data is delivered to the reciever is R/3 (this value was chosen arbitrarily to drive home the point that its strictly less than R/2)

        - thus, out of the 0.5R units of data transmitted, only 0.333R bytes/sec are original data and 0.166R bytes/sec are retransmitted data

    • Finally, let us consider the case that the sender may time out prematurely and retransmit a packet that has been delayed in the queue but not yet lost.
        
        - in this case, both the original data packet and the retransmission may reach the receiver

        - here the work done by the router in forwarding the retransmitted copy of the original packet was wasted, as the receiver will have already received the original copy of this packet

        - this illustrates another cost of a congested network; unneeded retransmissions by the sender in the face of large delays may cause a router to use its link bandwidth to forward unneeded copies of a packet

        - figure 3.44c shows that since each packet is forwarded twice, the throughput will have an asymptotic value of R/4 as the offered load approaches R/2

### Scenario 3: Four Senders, Routers with Finite Buffers, and Multihop Paths

    • Looking at figure 3.45, we assume that (1) each host uses a timeout/retransmission mechanism to implement a reliable data transfer service, (2) all hosts have the same value of λᵢₙ, and (3) all router links have capacity R bytes/sec.

    • We notice the Host A - Host C connection shares router R1 with the Host D - Host B connection and router 2 with the Host B - Host D connection

        - for small values of λᵢₙ, an increase in λᵢₙ results in an increase in λₒᵤₜ (throughput)
  
    • Next, consider Host A - Host C traffic arriving to router R2, which can have an arrival rate at R2 tht is at most R (the capacity of the link from R1 to R2 as R1 forwards traffic to R2)

        - if λ'ᵢₙ is extremely large for all connections (including the Host B - Host D connection), then the arrival rate of Host B - Host D  traffic at R2 can be much larger than that of the Host A - Host C traffic

        - the important point here is that because the Host A - Host C and Host B - Host D traffic must compete at router R2 for the limited amount of buffer space, the amount of  Host A - Host C traffic that successfully gets through R2 (that is, is not lost due to buffer overflow) becomes smaller and smaller as the offered load (λ'ᵢₙ) from Host B - Host D gets larger and larger
  
        - as the offered load approaches infinity, an empty buffer at R2 is immediately filled by a Host B - Host D packet, and the througput of the Host A - Host C traffic goes to zero!

        - see figure 3.46 for a graphical representation of this

    • The previous scenario illustrates a very important fact: every time a router drops a packet because its buffer is full, it's not just that one router failing.
    
        - it means that every previous router and every cable that packet already traveled through worked for nothing

        - the longer the path (the more "hops"), the worse this problem gets
  
        - in a 10-hop path, if a packet drops at hop 10, you’ve wasted the resources of 9 different links