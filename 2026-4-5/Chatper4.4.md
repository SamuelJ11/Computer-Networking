# 4.4: Generalized Forwarding and SDN

    • Let’s now consider a significantly more general “match-plus-action” paradigm, where the “match” can be made over multiple header fields associated with different protocols at different layers in the protocol stack.

        - The “action” part of generalized forwarding isn’t just sending a packet to port 'X', it can be any one of load-balancing, rewriting header values (NAT), intentionally dropping packets (firewall), deep-packet inspection (DPI), etc

    • In this section, a packet switch just means any device that forwards packets based on rules in a table:

        - this includes traditional routers, ethernet switches, etc

        - in generalized forwarding or SDN, the device is no longer “just a router”; it executes whatever action the controller installed

    • In practice, generalized match-plus-action capabilities are implemented via a remote controller that computes, installs, and updates match-plus-action tables.

    • Our following discussion of generalized forwarding will be based on Open-Flow, a highly visible standard that has pioneered the notion of the match-plus-action forwarding abstraction and controllers, as well as the SDN revolution more generally.

    • Each entry in the match-plus-action forwarding table, known as a flow table in OpenFlow, includes:

        (1) a set of header field values to which an incoming packet will be matched

        (2) a set of counters that are updated as packets are matched to flow table entries

        (3) a set of actions to be taken when a packet matches a flow table entry

    • The flow table is essentially an API, the abstraction through which an individual packet switch’s behavior can be programmed.

## 4.4.1: Match

    • The first observation we make is that OpenFlow’s match abstraction allows for a match to be made on selected fields from three layers of protocol headers;

        - recall that when data travels through a link, it is encapulated as follows:

            Application data → TCP/UDP segment → IP packet → Ethernet frame → physical signal

            *each layer encapsulates the entire PDU (headers + data) of the layer above it

    • Referencing figure 4.29, we can see that an OpenFlow-enabled device can equally perform as a router forwarding datagrams as well as a switch forwarding frames.

        - in OpenFlow 1.0, a flow rule can match on a set of packet-header fields (in this figure 12 shown), including things like:

            Ingress port
            Source MAC
            Destination MAC
            Ethernet type (e.g., IPv4, ARP)
            VLAN ID
            VLAN PRI
            Layer 3 (IP)
            Source IP
            Destination IP
            IP protocol (TCP/UDP/ICMP)
            Layer 4 (Transport)
            TCP/UDP source port
            TCP/UDP destination port

    • Flow table entries may also have wildcards, e.g., an IP address of 128.199.*.* in a flow table will match the corresponding address field of any datagram that has 128.199 as the first 16 bits of its address.

## 4.4.2: Action

    • Each flow table entry also has a list of zero or more actions that determine the processing that is to be applied to a packet that matches a flow table entry. If there are multiple actions, they are performed in the order specified in the list;

        - among the most important possible actions are forwarding, dropping, and modifying header fields

## 4.4.3: OpenFlow Examples of Match-plus-action in Action

    • Reference figure 4.30 to understand the discussion that follows.

### A First Example: Simple Forwarding

    • Suppose that the desired forwarding behavior is that packets from h5 or h6 destined to h3 or h4 are to be forwarded from s3 to s1, and then from s1 to s2 (thus completely avoiding the use of the link between s3 and s2 and ). 
    
        - the flow table entry in s1 would be:

        +-------------------------------------------------------------------------+
        |                         s1 Flow Table (Example 1)                       |
        +-------------------------------------------------------------+-----------+
        | Match                                                       | Action    |
        +-------------------------------------------------------------+-----------+
        | Ingress Port = 1 ; IP Src = 10.3.*.* ; IP Dst = 10.2.*.*    | Forward(4)|
        +-------------------------------------------------------------+-----------+
        | ...                                                         | ...       |
        +-------------------------------------------------------------+-----------+

        - of course, we’ll also need a flow table entry in s3 so that datagrams sent from h5 or h6 are forwarded to s1 over outgoing interface 3: 
  
        +-------------------------------------------------------------+------------+
        |                       s3 Flow Table (Example 1)             |            |
        +-------------------------------------------------------------+------------+
        | Match                                                       | Action     |
        +-------------------------------------------------------------+------------+
        | IP Src = 10.3.*.* ; IP Dst = 10.2.*.*                       | Forward(3) |
        +-------------------------------------------------------------+------------+
        | ...                                                         | ...        |
        +-------------------------------------------------------------+------------+

        - lastly, we’ll also need a flow table entry in s2 to complete this first example, so that datagrams arriving from s1 are forwarded to their destination, either host h3 or h4:

        +--------------------------------------------------------------+------------+
        |                        s2 Flow Table (Example 1)             |            |
        +--------------------------------------------------------------+------------+
        | Match                                                        | Action     |
        +--------------------------------------------------------------+------------+
        | Ingress port = 2 ; IP Dst = 10.2.0.3                         | Forward(3) |
        +--------------------------------------------------------------+------------+
        | Ingress port = 2 ; IP Dst = 10.2.0.4                         | Forward(4) |
        +--------------------------------------------------------------+------------+
        | ...                                                          | ...        |
        +--------------------------------------------------------------+------------+

### A Second Example: Load Balancing

    • As a second example, let’s consider a load-balancing scenario, where datagrams from h3 destined to 10.3.*.* are to be forwarded over the direct link between s2 and s1, while datagrams from h4 destined to 10.1.*.* are to be forwarded over the link between s2 and s3 (and then from s3 to s1). 
    
        *note that this behavior couldn’t be achieved with IP’s destination-based forwarding
        
        - in this case, the flow table in s2 would be:
  
        +--------------------------------------------------------------+------------+
        |                        s2 Flow Table (Example 2)             |            |
        +--------------------------------------------------------------+------------+
        | Match                                                        | Action     |
        +--------------------------------------------------------------+------------+
        | Ingress port = 3 ; IP Dst = 10.1.*.*                         | Forward(2) |
        +--------------------------------------------------------------+------------+
        | Ingress port = 4 ; IP Dst = 10.1.*.*                         | Forward(1) |
        +--------------------------------------------------------------+------------+
        | ...                                                          | ...        |
        +--------------------------------------------------------------+------------+

### A Third Example: Firewalling

    • As a third example, let’s consider a firewall scenario in which s2 wants only to receive (on any of its interfaces) traffic sent from hosts attached to s3.

        +--------------------------------------------------------------+------------+
        |                        s2 Flow Table (Example 3)             |            |
        +--------------------------------------------------------------+------------+
        | Match                                                        | Action     |
        +--------------------------------------------------------------+------------+
        | IP Src = 10.3.*.* IP Dst = 10.2.0.3                          | Forward(3) |
        +--------------------------------------------------------------+------------+
        | IP Src = 10.3.*.* IP Dst = 10.2.0.4                          | Forward(4) |
        +--------------------------------------------------------------+------------+
        | ...                                                          | ...        |
        +--------------------------------------------------------------+------------+

        - note that if there were no other entries in s2’s flow table, then only traffic from 10.3.*.* would be forwarded to the hosts attached to s2

    • The match-plus-action flow tables that we’ve seen in this section are actually a limited form of programmability, specifying how a router should forward and manipulate a datagram.


## 4.4.4: Middleboxes

    • So far we've learned that generalized forwarding allows a modern router to easily and naturally perform firewalling and load balancing with generalized “match plus action” operations:

        - these additional operations are performed by middleboxes (any intermediary box performing functions apart from normal, standard functions of an IP router)

    • Services provided by middleboxes can be categorized in three ways:

        (1) NAT Translation: private network addressing, rewriting datagram header IP addresses and port numbers.

        (2) Security Services: firewalls block traffic based on header-field values or redirect packets for additional processing, such as deep packet inspection (DPI)

        (3) Performance Enhancement: compression, content caching, and load balancing of service requests

    • Networks got complicated and expensive because of middleboxes; one solution is NFV (Network Function Virtualization):

        - instead of physical boxes, run network functions as software on general-purpose hardware
  
        *basically hardware-based networking -> software-based networking