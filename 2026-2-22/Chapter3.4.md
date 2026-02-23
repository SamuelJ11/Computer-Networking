# 3.4: Introduction and Transport-Layer Services

## 3.4.1: Building a Reliable Data Transfer Protocol

### Reliable Data Transfer over a Perfectly Reliable Channel: rdt 1.0

    • We now step through a series of protocols, each one becoming more complex, arriving at a flawless, reliable data transfer protocol.

        - there are seperate FSMs for the sender and reciever; in this model each only have one state

        - when no action is taken, a symbol looking like an upside down capital 'V' is shows above or below the horizontal

        - initial states are indicated by dashed arrows

    • The sending side of rdt simply accepts data from the upper layer via the rdt_send(data) event, creates a packet containing the data (via the action make_pkt(data)) and sends the packet into the channel. 

    • On the receiving side, rdt receives a packet from the underlying channel via the rdt_rcv(packet) event, removes the data from the packet (via the action extract(packet, data)) and passes the data up to the upper layer (via the action deliver_data(data)).
    
    • In this simple model all packet flow is from the sender to receiver; with a perfectly reliable channel there is no need for the receiver side to provide any feedback to the sender since nothing can go wrong! 

### Reliable Data Transfer over a Channel with Bit Errors: rdt 2.0

    • A more realistic model of the underlying channel is one in which bits in a packet may be corrupted. Such bit errors typically occur in the physical components of a network as a packet is transmitted, propagates, or is buffered

    • Control messages like ACK and NAK allow the receiver to let the sender know what has been received correctly, and has been recieved in error and thus requires repeating

        - In a computer network setting, reliable data transfer protocols based on such retransmission are known as ARQ (Automatic Repeat reQuest) protocols

    • Fundamentally, three additional protocol capabilities are required in ARQ protocols to handle the presence of bit errors:

        (1) Error detection     (checksum)
        (2) Receiver feedback   (ACK and NAK)
        (3) Retransmission      

    • The send side of rdt2.0 has two states:

        - in the leftmost state, the send-side protocol is waiting for data to be passed down fro the upper layer, then when the rdt_send(data) event occurs, the sender will create a packet (sndpck) containing the data to be sent along with the checksum, then send the packet via udt_send(sndpkt)

        - in the rightmost state, the sender protocol is waiting for an ACK or a NAK packet from the receiver.  
        
            (1) if an ACK packetg is recieved, the sender knows that the most recently transmitted packet has been recieved correctly and thus the protocol returns to the state of waiting for data from the upper layer

            (2) if a NAK is recieved, the protocol retransmits the last packet and wiats for an ACK or NAK to be returned by the reciever in response to the retransmitted data packet

    • It is important to note that when the sender is in the wait-for-ACK-or-NAK state, it cannot get more data from the upper layer; that is, the rdt_send() event can not occur; that will happen only after the sender receives an ACK and leaves this state. Thus, the sender will not send a new piece of data until it is sure that the receiver has correctly received the current packet. Because of this behavior, protocols such as rdt2.0 are known as stop-and-wait protocols.

    • The receiver side FSM for rdt2.0 still has a single state, just replying with either NAK or ACK depending on whether or not the received packet is corrupted.

    • The rdt 2.0 protocol fails to account for the possibility that the ACK or NAK packet could be corrupted!

        - the difficulty here is that if an ACK or NAK is corrupted, the sender has no way of knowing whether or not the receiver has correctly received the last piece of transmitted data

    • A simple solution to this new problem (and one adopted in almost all existing data transfer protocols, including TCP) is to add a new field to the data packet and have the sender number its data packets by putting a sequence number into this field.

        - the receiver then need only check this sequence number to determine whether or not the received packet is a retransmission

    • In a stop-and-wait protocol, only two numbers are needed for the sequence number: 0 and 1 (or “mod 2” numbering).

        - every time the sender sends a packt, the sequence number is incremented by 1 (mod 2)

        - if the first packet sent has a sequence number of 0, the receiver accepts it, delivers the data, and remembers that the last received sequence number is 0

        - the sender then waits for an acknowledgement before sending the next packet

        - if the ACK for packet 0 is lost, the sender retransmits packet 0

        - when the reciever sees packet 0 again, it notices the sequence number matches the last recieved number, meaning its a duplicate transmission, so it discards the data and resends an ACK

        - once the reciever sees a packet with sequence number 1, it knows this is new data, delivers it, updates the last recieved sequence number to 1, and sends and ACK for 1

        - the sender then increments the sequence number (mod 2) again, ready to send the next packet (0) and the process repeats

    • Our fixed version of rdt 2.1 sender and reciever FSMs each now have twice as many states as before; this is because the protocol state must now reflect whether the packet currently being sent (by the sender) or expected (at the receiver) should have a sequence number of 0 or 1

    • Our NAK-free reliable data transfer protocol for a channel with bit errors is rdt 2.2:

        - One subtle change between rdt 2.1 and rdt 2.2 is that the receiver must now include the sequence number of the packet being acknowledged by an ACK message (this is done by including the 0 or 1 argument in make_pkt() in the receiver FSM), and the sender must now check the sequence number of the packet being acknowledged by a received ACK message (this is done by including the 0 or 1 argument in isACK() in the sender FSM).

    • To clarify the last point, instead of sending a NAK, the receiver now just sends an ACK for the last correctly received packet:
    
        - if the sender gets duplicate ACKs for the same sequence number, it knows the next packet was lost or corrupted.

    • This approach avoids the need to have two types of ACK messages.

### Reliable Data Transfer over a Lossy Channel with Bit Errors: rdt 3.0

    • Suppose now that in addition to corrupting bits, the underlying channel can lose packets as well, a not-uncommon event in today’s computer networks (including the Internet):

        - two additional concerns must now be addressed by the protocol: how to detect packet loss and what to do when packet loss occurs

    • Suppose that the sender transmits a data packet and either that packet, or the receiver’s ACK of that packet, gets lost. In either case, no reply is forthcoming at the sender from the receiver.
    
        - if the sender is willing to wait long enough so that it is certain that a packet has been lost, it can simply retransmit the data packet . . . but how long should the sender wait?

    • The approach thus adopted in practice is for the sender to judiciously choose a time value such that packet loss is likely, although not guaranteed, to have happened. 

        - if an ACK is not received within this time, the packet is retransmitted. 
        
        - if a packet experiences a particularly large delay, the sender may retransmit the packet even though neither the data packet nor its ACK have been lost, introducing the possibility of duplicate data packets in the sender-to-receiver channel

    • Implementing a time-based retransmission mechanism requires a countdown timer that can interrupt the sender after a given amount of time has expired.

    • Because packet sequence numbers alternate between  and , protocol  is sometimes known as the alternating-bit protocol.

## 3.4.2: Pipelined Reliable Data Transfer Protocols

    • For the stop and wait protocol, the sender had a utilization (the fraction of time the sender is actually busy sending bits into the channel) of only 0.00027

        - That is, the sender was busy only 2.7 hundredths of one percent of the time! 

    • The solution to this particular performance problem is simple: Rather than operate in a stop-and-wait manner, the sender is allowed to send multiple packets without waiting for acknowledgments.

        - this is known as pipelining

    • Pipelining has several important consequences:

        (1) the range of sequence numbers must be increased, since each in-transit packet (not counting retransmissions) must have a unique sequence number and there may be multiple, in-transit, unacknowledged packets

        (2) the sender and receiver sides of the protocols may have to buffer more than one packet. Minimally, the sender will have to buffer packets that have been transmitted but not yet acknowledged. Buffering of correctly received packets may also be needed at the receiver

        (3) the range of sequence numbers needed and the buffering requirements will depend on the manner in which a data transfer protocol responds to lost, corrupted, and overly delayed packets. Two basic approaches toward pipelined error recovery can be identified: Go-Back-N and selective repeat

## 3.4.3: Go-Back-N (GBN)

    • In a Go-Back-N (GBN) protocol, the sender is allowed to transmit multiple packets (when available) without waiting for an acknowledgment, but is constrained to have no more than some maximum allowable number 'N' of unacknowledged packets in the pipeline.

    • Instead of just "0" and "1", we now have a long line of sequence numbers.
    
        (1) the Window: this is the bracketed section in the diagram of size 'N'. It represents the maximum number of packets the sender is allowed to have "in flight" (sent but not yet acknowledged).  As soon as the sender receives an ACK for the oldest packet in the window (base), the window "slides" forward to the right, allowing the sender to transmit a brand-new packet

        (2) The Limits of Sequence Numbers: the range of sequence numbers for packets that have been sent but aren't yet acknowledged is restricted to this window size.  This provides flow control and congestion control

    • If 'k' is the number of bits in the packet sequence number field (in the packet header), then the range of sequence numbers is this [0, 2ᵏ - 1]
    
        -With a finite range of sequence numbers, all arithmetic involving sequence numbers must then be done using (mod 2ᵏ) arithmetic

    • In our GBN protocol, an acknowledgment for a packet with sequence number 'n' will be taken to be a cumulative acknowledgment, indicating that all packets with a sequence number up to and including 'n' have been correctly received at the receiver. 

    • As in the stop-and-wait protocol, a timer will again be used to recover from lost data or acknowledgment packets. If a timeout occurs, the sender resends ALL packets that have been previously sent but that have not yet been acknowledged

        - this timer is a timer for the oldest transmitted but not yet acknowledged packet

        - if an ACK is received but there are still additional transmitted but not yet acknowledged packets, the timer is restarted

        - if there are no outstanding, unacknowledged packets, the timer is stopped

    • In our GBN protocol, the receiver discards a packet that has been received correctly but has a sequence number that is larger than 'expectedseqnum'. For example, if the GBN receiver is waiting for a packet with sequence number 'n', and a packet with sequence number 'n + 1' arrives, that packet is discarded by the GBN receiver even though the data in the arriving packet will eventually need to be delivered to the upper layer at the receiver. 

    • The most important takeaway is that GBN is the "skeleton" for TCP, and by learning GBN we are learning the four pillars of reliable data transfer:

        (1) sequence numbers: keeps packets in the right order
        (2) cumulative ACKs: one ACK can confirm multiple packets at once
        (3) checksums: detects if bits were flipped or corrupted during transit
        (4) timeout/retransmit: fixes the problem of lost packets by resending

## 3.4.4: Selective Repeat (SR)