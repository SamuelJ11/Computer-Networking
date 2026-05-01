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

### Open Shortest Path First (OSPF)

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

        (3) Integrated support for unicast and multicast routing: Multicast OSPF (MOSPF) provides simple extensions to OSPF to provide for multicast routing (the process of delivering 
                                                                  a single packet from a source to a specific group of receivers efficiently by replicating it only where paths to those receivers diverge in the network)

        (4) Support for hierarchlly within a single AS: An OSPF autonomous system can be configured hierarchically into areas; each area runs its own OSPF link-state routing algorithm,
                                                        with each router in an area broadcasting its link state to all other routers in that area. Within each area, one or more area border routers are responsible for routing packets outside the area. Lastly, exactly one OSPF area in the AS is configured to be the backbone area. 

# 5.4 Routing Among the ISPs: BGP

    • Since an inter-AS routing protocol involves coordination among multiple ASs, communicating ASs must run the same inter-AS routing protocol;

        - in fact, in the Internet, all ASs run the same inter-AS routing protocol, called the Border Gateway Protocol (BGP)

    • BGP is arguably the most important of all the Internet protocols, as it is the protocol that glues the thousands of ISPs in the Internet together.

## 5.4.1: The Role of BGP

    • As we have learned, for destinations that are within the same AS, the entries in the router’s forwarding table are determined by the AS’s intra-AS routing protocol. 

        - but what about destinations that are outside of the AS? enter BGP ...
  
    • In BGP, packets are not routed to a specific destination address, but instead to CIDRized prefixes, with each prefix representing a subnet or a collection of subnets.

        - for example, a destination may take the form 138.16.68/22 (1022 possible addresses)

        - thus, a router's forwarding table will have entries of the form (x, I) where 'x' is a prefix and 'I' is an interface number for one of the router's interfaces (ports)

    • As an inter-AS routing protocol, BGP provides each router a means to: 

        (1) Obtain prefix reachability information from neighboring AS's: BGP allows each subnet to advertise its existence to the rest of the Internet; if it weren’t for BGP, each subnet would be an isolated island—alone, unknown and unreachable by the rest of the Internet

        (2) Determine the “best” routes to the prefixes: To determine the best inter-domain route, the router will locally run a BGP route-selection procedure

## 5.4.2: Advertising BGP Route Information

    • Before we begin our discussion, its important to clarify that "gateway router" in this chapter means an AS border router that connects different Autonomous Systems, not the default gateway used by hosts inside a subnet.

        - gateway routers run BGP and sit at the border of the network

        - internal routers runs OSPF/IS-IS (link-state protocol) and only route traffic within the AS
  
    • Referencing Figure 5.8, lets consider the task of advertising reachability information for prefix 'x' to all of the other routers among AS1 and AS2:

        (1) AS3 sends a BGP message to AS2, saying that 'x' exists and is in AS3 (lets denote this message as AS3; x)

        (2) AS2 sends a BGP message to AS1, saying that 'x' exists and that you can get to 'x' by first passing through AS2 and then going to AS3 (we denote the message this time as AS2 AS3 x)

        - in this manner, each of the autonomous systems will not only learn about the existence of 'x', but also learn about a path of autonomous systems that leads to 'x'

    • In BGP, pairs of routers exchange routing information over semi-permanent TCP connections using port 179 (a well known TCP port used by BGP)

        - each such TCP connection, along with all the BGP messages sent over the connection, is called a BGP connection

        - a BGP connection that spans two As's is called an external BGP (eBGP) connection, and a BGP session between routers in the same AS is called an internal BGP (iBGP) connection

        * see figure 5.9 for an example of BGP connections (there is typically one eBGP connection for each link that directly connects gateway routers in different ASs)

    • Consider again advertising the reachability information for prefix 'x' to all routers in AS1 and AS2:

        (1) Gateway router 3a first sends an eBGP message "AS3; x" to gateway router 2c

        (2) Gateway router 2c then sends the iBGP message "AS3; x" to all of the other routers in AS2, including to gateway router 2a

        (3) Gateway router 2a then sends the eBGP message "AS2 AS3; x" to gateway router 1c

        (4) Finally, gateway router 1c sends an iBGP to send the message "AS2 AS3 x" to all the routers in AS1

## 5.4.3: Determining the Best Routes

    • When a router advertises a prefix (subnet) across a BGP connection, it includes with the prefix several BGP attributes;

        - a prefix along with its attributes is called a route

        (1) the AS-PATH attribute contains the list of ASs through which the advertisement has passed

            * BGP routers also use the AS-PATH attribute to detect and prevent looping advertisements; specifically, if a router sees that its own AS is contained in the path list, it will reject the advertisement

        (2) the NEXT-HOP attribute has a subtle but important use; the NEXT-HOP is the IP address of the router interface that BEGINS the AS-PATH

            * in figure 5.10, the NEXT-HOP attribute for the route "AS2 AS3; x" from AS1 to 'x' that passes through AS2 is the IP address of the left interface on router 2a

### Hot Potato Routing
  
    • In hot potato routing, the route chosen is that route with the least cost to the NEXT-HOP router beggining that route;

        - For example, suppose that cost is defined as the number of links traversed; then the least cost from router 1b to router 2a is 2, the least cost from router 1b to router 2d is 3, and router 2a would therefore be selected

    • Reference figure 5.11 to understand the discussion that follows:

        - it is important to note that when adding an outside-AS prefix into a forwarding table, both the inter-AS routing protocol (BGP) and the intra-AS routing protocol (e.g., OSPF) are used

        - this figure is describing how a router decides where to send traffic to a destination subnet that exists outside its AS

        (1) Learn from inter-AS protocol that subnet 'x' is reachable via multiple gateways (this is BGP's job)

        (2) Use intra-AS protocol to determine costs to each gateway (this is OSPF within the AS)

        (3) Hot potato routing: choose the gateway with smallest cost

            * even if one gateway might lead to a "better" path, we still are making this nearsighted decision

        (4) Update forwarding table

            - if destination = subnet 'x' → send to Gateway 'y' via interface 'I'

    • From the previous example, we can see that in the name “hot potato routing,” a packet is analogous to a hot potato that is burning in your hands:

        - because it is burning hot, you want to pass it off to another person (another AS) as quickly as possible

        - hot potato routing is thus a selfish algorithm—it tries to reduce the cost in its own AS while ignoring the other components of the end-to-end costs outside its AS

### Route-Selection Algorithm

    • In practice, BGP uses an algorithm that is more complicated than hot potato routing, but nevertheless incorporates hot potato routing. 

    • If there are two or more routes to the same prefix, then BGP sequentially invokes the following elimination rules until one route remains:

        (1) A route is assigned a local preference value as one of its attributes (the value of the local preference attribute is a policy decision that is left entirely up to the AS’s network administrator, and the routes with the highest local preference values are selected)

        (2) From the remaining routes, the route with the shortest AS-PATH is selected

        (3) From the remaining of THOSE routes, hot potato routing is used, that is, the route with the closest NEXT-HOP router is selected

        * if more than one route still remains, the router uses BGP identifiers to select the route

    • So we see that with the above route-selection algorithm, BGP is no longer a selfish algorithm—it first looks for routes with short AS paths (thereby likely reducing end-to-end delay).

## 5.4.4: IP-Anycast

    • In addition to being the Internet’s inter-AS routing protocol, BGP is often used to implement the IP-anycast service, which is commonly used in DNS:

        - When a user wants to access this replicated content from a CDN, it is desirable to point the user to the “nearest” server with the replicated content; BGP’s route-selection algorithm provides an easy and natural mechanism for doing so

        - for example, A CDN uses the same IP address at multiple physical servers, advertises that same IP prefix via BGP from multiple locations, and routers then pick a “best” route based on BGP policy (often AS-path length)

        - the effect is different clients around the world get routed to different CDN servers without the client explicitly choosing

        * each router independently picks the “best route” to the destination IP based on BGP rules, and those rules depend on topology and policy

    • IP-anycast is widely used in DNS because DNS queries are stateless and short-lived, but CDNs usually avoid it for web traffic because TCP connections require stable, consistent routing to a single server instance.

## 5.4.5: Routing Policy

    • Recall that BGP routers pick routes in this order:

        1. Highest local preference 
        2. Shortest AS-path 
        3. Hot potato routing 

        * Reference figure 5.13 and view the discussion that follows; its pretty intuitive but read carefully

        * An important rule of thumb followed by commercial ISPs is that any traffic flowing across an ISP’s backbone network must have either a source or a destination (or both) in a network that is a customer of that ISP; otherwise the traffic would be getting a free ride on the ISP’s network