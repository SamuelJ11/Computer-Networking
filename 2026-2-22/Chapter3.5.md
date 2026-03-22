# 3.5: Connection-Oriented Transport: TCP

## 3.5.1: The TCP Connection

    • A TCP connection provides a full-duplex service; if there is a TCP connection between Process A on one host and Process B on another host, then application-layer data can flow from Process A to Process B at the same time as application-layer data flows from Process B to Process A.

    • A TCP connection is also always point-to-point, that is, between a single sender and a single receiver. 

    • Referencing figure 3.27, TCP directs data to the connection's send buffer, and from time to time TCP grabs chunks of data from the send buffer and passes it to the network layer

    • The maximum amount of data that can be grabbed and placed in a segment is limited by the maximum segment size (MSS)

        - the MSS is set by first determining the length of the largest link-layer frame that can be sent by the local sending host (aka the maximum transmission unit, MTU) and then setting MSS to ensure that a TCP segment (when encapsulated in an IP packet) plus the TCP-IP header length (typically 40 bytes in total) will fit into a single link-layer frame

    • Ethernet protocols have an MTU of 1500 bytes, so subtracting the TCP/IP header length yields the 1460 byte MSS.

        - for this reason, the MSS is the maximum amount of application-layer data in the segment, not the maximum size of the TCP segment including headers

    • Critically, segments are separately encapsulated, meaning that even if one big file is broken into 100 segments, the network layer treats each resulting IP datagram as an independent traveler.

        - One datagram might take a different physical path through the internet than another

        - IP doesn't know they are related; only TCP at the destination is smart enough to strip off the IP "envelopes" and put the segments back in the right order

## 3.5.2: TCP Segment Structure

    • The TCP segment consists of header fields and a data field. 

    • When TCP sends a large file, such as an image as part of Web page, it typically breaks the file into chunks of size MSS.

        - interactive applications, however, often transmit dta chunks that are smaller than the MSS; e.g., Telnet and SSH
    
    • As with UDP, the header includes source and destination port numbers, which are used for multiplexing/demultiplexing data from/to upper-layer applications.

        - a TCP segment header also contains the following fields:

            (1) the 32-bit 'sequence number' and 'acknowledgment number' field

            (2) a 16-bit 'receive window' field for flow control

            (3) a 4-bit 'header length' field specifies the length of the TCP header in 32-bit words (the TCP header cna be of variable length due to the TCP options field)

            (4) the optional and variable-length 'options' field is used when a sender and receiver negotiate the maximum segment size (MSS) or as a window scaling factor for use in high-speed networks

            (5) the 6-bit Flag field in a TCP header acts as a set of "on/off" switches that tell the receiving computer how to handle the segment:

                - ACK: confirms that the acknowledgment number in the header is valid

                - SYN: used to initiate a connection

                - FIN: used to gracefully close a connection

                - RST: used to abruptly abort a connection due to an error

                - ECE/CWR - used to notify the other side that the network is getting crowded 

                - PSH/URG - historically used to prioritize data delivery, rarely used in modern networking

### Sequence Numbers and Acknowledgment Numbers

    • TCP views data as an unstructured, but ordered, stream of bytes. 

        - sequence numbers are over the stream of transmitted bytes and not over the series of transmitted segments

        - the sequence number for a segment is therefore the byte-stream number of the first byte in the segment

        - each sequence number is inserted in the sequence number field in the header of the appropriate TCP segment

        * see figure 3.29 and the preceeding paragraph for a clearer explanation

    • The acknowledgment number that Host A puts in its segment is the sequence number of the next byte Host A is expecting from Host B. 

    • Because TCP only acknowledges bytes up to the first missing byte in the stream, TCP is said to provide cumulative acknowledgments:

        - as an example of this, suppose Host A has received one segment from Host B containing bytes 0-535 and another segment containing bytes 900-1000. For some reason Host A has not yet received bytes 536-899

        - Host A is still waiting for byte 536 (and beyond) in order to re-create B’s data stream. Thus, A’s next segment to B will contain 536 in the acknowledgment number field

### Telnet: A Case Study for Sequence and Acknowledgment Numbers

    • Honestly the textbook explains this short section better than I can, it's a quick read just view figure 3.30 to tie it all together.

## 3.5.3: Round-Trip Time Estimation and Timeout

    • TCP, like the rdt protocol in section 3.4, uses a timeout/retransmit mechanism to recover from lost segments. 

        - clearly this timeout should be larger than the connection's RTT . . . but how much longer?

### Estimating the Round-Trip Time

    • In order to estimate a typical RTT, it is therefore natural to take some sort of average of the recorded RTT values. TCP maintains an average, called 'EstimatedRTT', of the individual RTT values. Upon obtaining a new RTT, TCP updates EstimatedRTT according to the following formula:

        Estimated RTT = (1 - α).EstimatedRTT + α.(RTT)

        - this states that the new estimate is mostly based on the rolling average (1 - α), plus a tiny bit of the most recent RTT (α)

        - the recommended value of α is 0.125

    • In addition to having an estimate of the RTT, it is also valuable to have a measure of the variability of the RTT

        - here we define 'DevRTT to be an estimate of how much Sample RTT typically deviates from EstimatedRTT via the formula:

            DevRTT = (1 - β).DevRTT + (β).abs(SampleRTT - EstimatedRTT)

        - the recommended value of β is 0.25

### Setting and Managing the Retransmission Timeout Interval

    • It is therefore desirable to set the timeout equal to the EstimatedRTT plus some margin

        - the margin shoud be large when DevRTT is large, and small with DevRTT is small

    • The final formula for the timeout interval is thus:

        TimeoutInterval = EstimatedRTT + 4.(DevRTT)

    • An initial TimeoutInterval value of 1 second is recommended, and when a timeout occurs, the value of TimeoutInterval is doubled to avoid a premature timeout occurring for a subsequent segment that will soon be acknowledged.
    
        - however, as soon as a segment is received and TimeoutInterval is updated, the TimeoutInterval is again computed using the formula above
    
## 3.5.4: Reliable Data Transfer

    • TCP creates a reliable data transfer service on top of IP’s unreliable best-effort service.

    • This simplified TCP sender pseudocode in figure 3.32 describes a "finite state machine" that ensures data is delivered reliably by tracking what has been sent versus what has been acknowledged
    
        (1) NextSeqNum: the sequence number of the next byte of data to be sent. This is used to label new segments so the receiver can put them in the correct order.
        
        (2) SendBase: The sequence number of the oldest unacknowledged byte. Think of this as the "back wall" of the sender's window. Anything before SendBase is confirmed; anything between SendBase and NextSeqNum is currently "in flight" across the network
        
        (3) TCP wraps the data into a segment using the current NextSeqNum
        
        (4) if the timer is not already running, TCP starts it; the timer always tracks the oldest segment in flight
        
        (5) NextSeqNum is increased by the length of the data sent; this prepares the sender for the next chunk of data from the application
        
        (6) in the event of a timeout, TCP retransmits the single segment with the smallest sequence number (the one at SendBase).
        
            - here the timer is restarted immediately because TCP assumes the segment was lost or delayed. By only resending the oldest segment, it avoids flooding the network while still nudging the receiver to respond
            
        (7) in the event of an ACK Received (with value y), TCP checks if the acknowledgment value 'y' is greater than the current SendBase.
        
            - if'y' is larger, SendBase is moved forward to equal 'y'; sliding the window forward, effectively clearing out confirmed data from the sender's memory

            - if there are still segments in flight (meaning SendBase hasn't caught up to NextSeqNum), the timer is restarted to track the new oldest unacknowledged segment

### Fast Retransmit

    • When a segment is lost, the long timeout period forces the sender to delay resending the lost packet, thereby increasing the end-to-end delay.
    
        - fortunately, the sender can often infer packet loss well before the timeout event occurs by noting so-called duplicate ACKs(an ACK is an ACK that reacknowledges a segment for which the sender has already received an earlier acknowledgment)

    • When a TCP receiver receives a segment with a sequence number that is larger than the next, expected, in-order sequence number, it detects a gap in the data stream (a missing segment).
    
        - this gap could be the result of lost or reordered segments within the network, and since TCP uses cumulative ACKs, when a gap is detected, the TCP receiver simply reacknowledges (that is, generates a duplicate ACK for) the last in-order byte of data it has received

    • To make this more concrete, lets suppose the sender has sent Segments 2, 3, 4, and 5, and Segment 2 is lost:

        Segment 3 arrives: Receiver says "ACK 2" (I'm still waiting for 2).

        Segment 4 arrives: Receiver says "ACK 2" again.

        Segment 5 arrives: Receiver says "ACK 2" a third time.

        - this brings us to the "Rule of Three":
      
            -- sometimes packets just get reordered (3 arrives before 2 because it took a faster "shortcut" through a different router)

            -- but if the sender retransmitted the moment it saw one duplicate ACK, it would waste a lot of bandwidth on packets that weren't actually lost, just a bit late

            -- by waiting for three duplicates (the original ACK + 3 extras), TCP is reasonably sure Segment 2 is actually lost, not just stuck in traffic

    • In the case that triple duplicate ACKs are received, the TCP sender performs a fast retransmit, retransmitting the missing segment before that segment’s timer expires.

### Go-Back-N or Selective Repeat?

    • TCP is smarter than a standard GBN protocol. If you send 5 segments and the first one is lost, a standard GBN would retransmit all 5 segments; TCP does not.

        - TCP usually only retransmits one segment — the one that actually timed out

        - while standard GBN throws away out-of-order segments, many TCP implementations buffer them so they don't have to be resent later

        -  If the ACK for segment 'n' is lost, but the ACK for 'n + 1' arrives before the timeout, TCP does not retransmit segment 'n'; it realizes that if the receiver got 'n + 1', it must have received 'n' successfully

    • In short, TCP uses the simple tracking of GBN (e.g., SendBase and NextSeqNum) but the efficient retransmission style of SR.

## Flow Control

    • Recall that the hosts on each side of a TCP connection set aside a receive buffer for the connection.

        - if the application is relatively slow at reading the data, the sender can very easily overflow the connection’s receive buffer by sending too much data too quickly

    • TCP provides a flow-control service to its applications to eliminate the possibility of the sender overflowing the receiver’s buffer.

        - not to be confused with congestion control

    • For the discussion that follows, we assume that TCP discards out-of-order segments

    • TCP provides flow control by having the sender maintain a variable called the receive window (discussed in 3.4).

    • Because TCP is full-duplex, the sender at each side of the connection maintains a distinct receive window.

    • To understand figure 3.36, lets make an analogy involving a bucket:

        - imagine the RcvBuffer is a physical bucket: the blue box is data that has arrived from the network but hasn't been "read" by the application yet.
        
        - the yellow bos is the empty space left in the bucket.
        
        - to keep track of the space, TCP uses three main numbers:
        
            (1) LastByteRcvd: the total amount of data that has entered the bucket
            
            (2) LastByteRead: the total amount of data the application has actually pulled out of the bucket
            
            (3) RcvBuffer: the total size of the bucket.
            
        - the amount of data currently sitting in the buffer is calculated via (LastByteRcvd - LastByteRead)
        
        - the yellow box (aka rwnd) is calculated via RcvBuffer - [LastByteRcvd - LastByteRead]
        
        - in other words, the spare room (rwnd) is the total size of the bucket (RcvBuffer) minus the data already sitting in it (LastByteRcvd - LastByteRead)

    • Because the spare room changes with time, rwnd is dynamic.

    • In this example, Host B tells Host A how much spare room it has in the connection buffer by placing its current value of rwnd in the recieve window field of every segment it sends to A

    • Host A in turn keeps track of two variables, 'LastByteSent' and 'LastByteAcked', which have obvious meanings.

        - the difference between these two variables (LastByteSent - LastByteAcked) is the amount of unacknowledged data that A has sent into the connection

        - by keeping the amount of unacknowledged data less than the value of rwnd, Host A is assured that it is not overflowing the recieve buffer at Host B:

            LastByteSent - LastByteAcked <= rwnd

    • There is one minor technical problem with this scheme:

        - suppose Host B’s receive buffer becomes full so that rwnd = 0. After advertising rwnd = 0 to Host A, also suppose that B has nothing to send to A

        - as the application process at B empties the buffer, TCP does not send new segments with new rwnd values to Host A; indeed, TCP sends a segment to Host A only if it has data to send or if it has an acknowledgment to send

        - therefore, Host A is never informed that some space has opened up in Host B’s receive buffer and Host A is blocked, waiting indefinetely for data from Host B

        - to break this silence, TCP uses a zero-window Probe; Host A periodically sends a tiny "probe" packet containing exactly 1 byte of data

        - Host B is forced to respond to this 1-byte packet with an acknowledgment, and in that acknowledgement, Host B must include its current rwnd

        - eventually the buffer will begin to empty and the acknowledgments will contain a nonzero rwnd value (meaning Host B now has space in its buffer)

    • Unlike TCP, UDP has no "speed limit" or "spare room" updates, so the sender just blasts data as fast as it wants. 
    
        - if the receiver's application is too slow to pull data out of the buffer, the buffer simply overflows and the extra data is permanently deleted

## 3.5.6: TCP Connection Management

    • Suppose a process running in one host (client) wants to initiate a connection with another process in another host (server):

        (1) the client-side TCP first sends a special TCP segment with the one of the flag bits (SYN bit) set to 1; this is SYN segment

            - the segment is encapsulated within an IP datagram and sent to the server with an initial sequence number 'client_isn'

        (2) the server extracts the TCP SYN segment from the datagram, allocates the TCP buffers and variables to the connection, and sends a connection-granted segment to the client TCP

            - in this "connection granted" segment, the SYN bit is also set to 1, the acknowledgement field of the TCP segment header is set to client_isn + 1, and the server chooses its own sequence number 'server_isn" and puts this value in the sequence number field of the TCP segment header

            - this is formally referred to as the SYNACK segment

        (3) upon recieving the SYNACK segment, the client allocates buffers and variables to the connection

            - the client host acknowledges the server’s connection-granted segment by putting the value 'server_isn + 1' in the acknowledgment field of the TCP segment header, and the SYN bit is set to 0

    • Since 3 packets were sent between the two hosts to establish a connection, this processes is referred to as the "three-way handshake"

    • If the client and server have previously communicated, it’s possible to use a technique known as “fast open” to reduce the handshaking time to zero! 

    • The key idea behind fast open, also sometimes referred to as 0-RTT handshaking, is that during a traditional three-way handshake, a client can also request that the server provide it with a fast-open cookie, which encodes all of the connection information needed in a future connection.

        - the next time the client wants to establish a connection with this server, it sends the fast-open cookie together with application-layer data in its very first message to the server

    • When the client application process issues a close command, the client TCP sends a special TCP segment with the 'FIN' flag bit set to 1.

        - when the server receives this segment, it sends the client an acknowledgment segment in return. The server then sends its own shutdown segment, which has the FIN bit set to

    • During the life of a TCP connection, the TCP protocol running in each host makes transitions through various TCP states (see figures 3.39 and 3.40)