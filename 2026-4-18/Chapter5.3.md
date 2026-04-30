# 5.3: Intra-AS Routing in the Internet: OSPF

    • In our study of routing algorithms so far, we've viewed the network simply as a collection of interconnected routers in which each router is indistinguishable from the other;

        - this model and its view of a homogenous set of routers all executing the same routing algorithm is simplistic for two important reasons, namely

            (1) as the number of routers becomes large, the overhead involved in communicating, computing, and storing routing information becomes prohibitive

            (2) the Internet is a network of ISPs, with each ISP consisting of its own network of routers; an ISP generally desires to operate its network as it pleases or to hide aspects of its network’s internal organization from the outside

    • Both of these problems can be solved by organizing routers into autonomous systems (ASs), with each AS consisting of a group of routers that are under the same administrative control.

        - some ISPs partition thier network into multiple AS's

        - an autonomous system is identified by its globally unique AS number, which, like IP addresses, are assigned by ICANN (Internet Corporation for Assigned Names and Numbers)

    • Routers within the same AS all run the same routing algorithm and have information about each other:

        - The routing algorithm running within an autonomous system is called an intra-autonomous system routing protocol

## Open Shortest Path First (OSPF)

    • OSPF routing and its closely related cousin, IS-IS (Intermediate System to Intermediate System) are widely used for intra-AS routing in the Internet.

    • OSPF is a link-state protocol that uses flooding of link-state information and a Dijkstra’s least-cost path algorithm;

        - with OSPF, each router constructs a complete topological map of the entire AS, then each router runs Dijkstra's shortest path algorithm to determine a shortest path tree to all subnets, with itself as the root node

        - to make this more clear, enjoy this ASCII drawing by ChatGPT
  
            Router A ---- Router B ---- Router C
                |             |
             Subnet1       Subnet2

            * routers are transit points, subnets are endpoints (where traffic is actually delivered)

    • Individual link costs are configured by the network administrator:

        - the administrator might choose to set all link costs to 1, thus achieving minimum-hop routing, or might choose to set the link weights to be inversely proportional to link capacity in order to discourage traffic from using low-bandwidth links

        - OSPF provides the mechanisms (protocol) for determining least-cost path routing for the given set of link weights

    • OSPF is a link-state routing protocol where routers flood link information (LSAs) throughout the entire autonomous system, periodically and on changes, build identical network maps, and independently compute shortest paths using Dijkstra — all carried directly in IP (protocol 89) with OSPF handling its own reliability and neighbor discovery via HELLO messages.

    • Some of the advances embodied in OSPF include the following:

        (1) Security: exchanges between OSPF routers (for example, link-state updates) can be authenticated, and two types of authentication (simple and MD5) can be configured.
                      MD5 authentication is based on shared secret keys that are configured in all the routers (for each OSPF packet that it sends, the router computes the MD5 hash of the content of the OSPF packet appended with the secret key), then the router includes the resulting hash in the OSPF packet.  The recieving router, using the preconfigured secret key, will compute an MD5 hash of the packet and compare it with the hash value the packet carries, thus verifying the packet's authenticity

        (2) Muliple same-cost paths: When multiple paths to a destination have the same cost, OSPF allows multiple paths to be used (that is, a single path need not be chosen for carrying
                                     all traffic when multiple equal-cost paths exist)

        (3) Integrated support for unicast and multicast routing: Multicast OSPF (MOSPF) provides simple extensions to OSPF to provide for multicast routing (the process of delivering a single packet from a source to a specific group of receivers efficiently by replicating it only where paths to those receivers diverge in the network)

        (4) Support for hierarchlly within a single AS: An OSPF autonomous system can be configured hierarchically into areas; each area runs its own OSPF link-state routing algorithm,
                                                        with each router in an area broadcasting its link state to all other routers in that area. Within each area, one or more area border routers are responsible for routing packets outside the area. Lastly, exactly one OSPF area in the AS is configured to be the backbone area. 


