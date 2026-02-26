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

   