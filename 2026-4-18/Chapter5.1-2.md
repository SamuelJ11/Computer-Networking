# 5.1: Introduction

    • Recall that in the previous chapter we learned that there are 2 possible approaches for how flow tables are computed, maintained and installed:

        (1) Per-router control (each router has a routing component that communicates with the routing components in other routers to compute the values for its forwarding tables)

        (2) Logically centralized control (a controller computes and distributes the forwarding tables to be used by each and every router, aka control agent)

# 5.2: Routing Algorithms

    • In this section, we’ll study routing algorithms, whose goal is to determine good paths (equivalently, routes), from senders to receivers, through the network of routers.

    • A graph is used to formulate routing problems:

        - recall that a graph G = (N, E) is a set of N nodes and a collections of E edges, where each edge is a pair of nodes from N

        - in the context of networking, the nodes in the graph represent routers, and the edges are the physical links between them

    • When we study the BGP inter-domain routing protocol, we’ll see that nodes represent networks, and the edge connecting two such nodes represents direction connectivity (know as peering) between the two networks.

    • Referencing figure 5.3, an edge also has a value representing its cost; typically the physical length of the corresponding link;

        - for any edge (x, y) in 'E', we denote c(x, y) as the cost of the edge between nodes 'x' and 'y'

        - if the pair (x, y) does not belong to 'E', we set c(x, y) to 'inf'

        - also, we will only consider undirected graph, and a node 'y' is said to be a neighbor of node 'x' if (x, y) belongs to E

    • A natural goal of a routing algorithm is to identify the least costly paths between sources and destinations.

        - recall that a path in a graph is a sequence of nodes (x₁, x₂, ..., xₚ) such that each of the pairs (x₁, x₂), (x₂, x₃), ..., (xₚ₋₁, xₚ) are edges in 'E'

        - the cost a path is simply the sum of all the edge costs along the path, or in other words, c(x₁, x₂) + c(x₂, x₃), + ... c(xₚ₋₁, xₚ)

    • Given any two nodes 'x' and 'y', there are typically many paths between them, one or more of which is a least-cost path:

        - note that if all edges in the graph have the same cost, the least-cost path is also the shortest path (that is, the path with the smallest number of links between the source and the destination)

    • Broadly, one way in which we can classify routing algorithms is according to whether they are centralized or decentralized.

        (1) a centralized routing algorithm computes the least-cost path between a source and destination using complete, global knowledge about the network. the key distinguishing feature here, however, is that the algorithm has complete information about connectivity and link costs. Algorithms with global state information are often referred to as link-state (LS) algorithms

        (2) in a decentralized routing algorithm, the calculation of the least-cost path is carried out in an iterative, distributed manner by the routers. each node begins with only the knowledge of the costs of its own directly attached links.  these algorithms are called distance-vector algorithms (DV) because each node maintains a vector of estimates of the costs (distances) to all other nodes in the network

    • A second broad way to classify routing algorithms is according to whether they are static or dynamic:

        (1) in static routing algorithms, routes change very slowly over time, often as a result of human intervention (for example, a human manually editing a link costs)

        (2) dynamic routing algorithms change the routing paths as the network traffic loads or topology change

    • A third way to classify routing algorithms is according to whether they are load-sensitive or load-insensitive. 

        - in a load-sensitive algorithm, link costs vary dynamically to reflect the current level of congestion in the underlying link:

        - if a high cost is associated with a link that is currently congested, a routing algorithm will tend to choose routes around such a congested link

        - today’s Internet routing algorithms (such as RIP, OSPF, and BGP) are load-insensitive, as a link’s cost does not explicitly reflect its current (or recent past) level of congestion

# 5.2.1: The Link-State (LS) Routing Algorithm

    • Recall that in a link-state algorithm, the network topology and all link costs are known, that is, available as input to the LS algorithm. 

        - in practice, this is accomplished by having each node broadcast link-state packets to all other nodes in the network, with each link-state packet containing the identities and costs of its attached links

        - the result of the nodes’ broadcast is that all nodes have an identical and complete view of the network

    • The link-state routing algorithm we present below is known as Dijkstra’s algorithm, named after its inventor.

    • The best way to understand is to work through the example network in Figure 5.3 (recreated below) and compute the least-cost paths from  to all possible destinations.
                    
            +--------5--------+
            |                 |
            |      [V]---3---[W]-
            |     / |        / | \
           [U]---2  |       /  |  5
            \       2      3   1   \
             1      |  ___/    |   [Z]
              \     | /        |   /
               +---[X] ---1---[Y]-2
                               
        *first we define the following:

        - D(v) is the cost of the least-cost path from the source node 'u' to destination 'v' as of the CURRENT iteration of the algorithm
        - p(v) is the previous node (neighbor of v) along the current least-cost path from the source 'u' to 'v'
        - N' is a subset of nodes; v is in N' if the least-cost path from the source to 'v' is definitively known

        (1) in the initialization step, the currently known least-cost paths from 'u' to its directly attached neighbors 'v', 'x' and 'w' are initialized to 2, 1, and 5, respectively.  note in particular that the cost to 'w' is 5 even though a lesser-cost path exists.  the costs to 'y' and 'z' are set to infinity because they are not directly connected to 'u'

            +------+---------+----------+----------+----------+----------+----------+
            | step |   N'    | D(v),p(v)| D(w),p(w)| D(x),p(x)| D(y),p(y)| D(z),p(z)|
            +------+---------+----------+----------+----------+----------+----------+
            |  0   | u       |   2, u   |   5, u   |   1, u   |    inf   |    inf   |
            |  1   | ux      |          |          |          |          |          |
            |  2   | uxy     |          |          |          |          |          |
            |  3   | uxyv    |          |          |          |          |          |
            |  4   | uxyvw   |          |          |          |          |          |
            |  5   | uxyvwz  |          |          |          |          |          |
            +------+---------+----------+----------+----------+----------+----------+

        (2) in the first iteration, we look among those nodes not yet added to the set N' and find that node with the least cost as of the end of the previous iteration.  Since N' is currently empty, and the path from 'u' to 'x' has the least cost among all other nodes, we add 'x' to the set N'.  Now, D(v) is updated for all nodes 'v', yielding the results shown in the second table

            +------+---------+----------+----------+----------+----------+----------+
            | step |   N'    | D(v),p(v)| D(w),p(w)| D(x),p(x)| D(y),p(y)| D(z),p(z)|
            +------+---------+----------+----------+----------+----------+----------+
            |  0   | u       |   2, u   |   5, u   |   1, u   |    inf   |    inf   |
            |  1   | ux      |   2, u   |   4, x   |          |   2, x   |    inf   |
            |  2   | uxy     |          |          |          |          |          |
            |  3   | uxyv    |          |          |          |          |          |
            |  4   | uxyvw   |          |          |          |          |          |
            |  5   | uxyvwz  |          |          |          |          |          |
            +------+---------+----------+----------+----------+----------+----------+

            *importantly, since the cost of the path to 'w' (which was 5 at the end of the initialization) through node 'x' is found to have a cost of 4, this lower-cost path is selected and w's predecessor along the shortest path from 'u' is set to 'x'

            *similarly, the cost to 'y' (though x) is computed to be 2, and the table is updated accordingly

        (3) in the second iteration, nodes 'v' and 'y' are found to have least-cost paths of 2, and we break the tie arbitrarily and add 'y' to the set N' so that now N' contains 'u', 'x' and 'y'.  the cost to the remaining nodes not yet in N' (v, w, and z) are updated and so on.  The final table is given below

            +------+---------+----------+----------+----------+----------+----------+
            | step |   N'    | D(v),p(v)| D(w),p(w)| D(x),p(x)| D(y),p(y)| D(z),p(z)|
            +------+---------+----------+----------+----------+----------+----------+
            |  0   | u       |   2, u   |   5, u   |   1, u   |    inf   |    inf   |
            |  1   | ux      |   2, u   |   4, x   |          |   2, x   |    inf   |
            |  2   | uxy     |   2, u   |   3, y   |          |          |   4, y   |
            |  3   | uxyv    |          |   3, y   |          |          |   4, y   |
            |  4   | uxyvw   |          |          |          |          |          |
            |  5   | uxyvwz  |          |          |          |          |          |
            +------+---------+----------+----------+----------+----------+----------+

    • When the LS algorithm terminates, we have, for each node, its predecessor along the least-cost path from the source node, and for each predessor ITS predecessor, and so on so that we can construct the entire path from the source to all destinations

        - referencing figure 5.4,  the forwarding table in a node, say node 'u', can then be constructed from this information by storing, for each destination, the next-hop node on the least-cost path from 'u' to the destination

    • Having studied the Link State (LS) algorithm, let’s consider the other major routing algorithm that is used in practice today—the distance-vector routing algorithm.

## 5.2.2: The Distance-Vector (DV) Routing Algorithm

    • The distance-vector (DV) algorithm is iterative, asynchronous, and distributed:

        - it is istributed in that each node receives some information from one or more of its directly attached neighbors, performs a calculation, and then distributes the results of its calculation back to its neighbors

        - it is iterative in that this process continues on until no more information is exchanged between neighbors

        - it is asynchronous in that it does not require all of the nodes to operate in lockstep with each other

    • Before we present the DV algorithm, it will prove beneficial to discuss an important relationship that exists among the costs of the least-cost paths.

        - let dₓ(y) be the cost of the least-cost path from node 'x' to node 'y'

    • The least costs are related by the celebrated Bellman-Ford equation:

        - dₓ(y) = minᵥ{c(x, v) + dᵥ(y)}

        *minᵥ is the minimum over all possible neighbors 'v' of node 'x'

        - to make this more concrete, imagine 'x' has neighbors v₁, v₂, v₃; then you’re computing:

            cost via v₁: c(x,v₁) + dv₁(y) "cost from x to v₁ plus the cost from v₁ to y"
            cost via v₂: c(x,v₂) + dv₂(y) "cost from x to v₂ plus the cost from v₂ to y"
            cost via v₃: c(x,v₃) + dv₃(y) "cost from x to v₃ plus the cost from v₃ to y"
        
        - then we are picking the smallest of those via minᵥ{c(x, v) + dᵥ(y)}

    • The solution to the Bellman-Ford equation provides the entries in node 'x's' forwarding table in the above example.

    • The basic idea is as follows:

        (1) each node 'x' begins with Dₓ(y), an estimate of the cost of the least-cost path from itself to some destination node 'y', for all nodes, 'y', in 'N'

        (2) let Dₓ = [Dₓ(y) : y in N] be node x's distance vector, which is the vector of cost estimates from x to all other nodes, 'y', in 'N' (a list of numbers, where each number is the estimated distance from 'x' to some node 'y' in the network)

    • With the DV algorithm, each node 'x' maintains the following routing information:

        (1) for each neighbor v, the cost c(x, v) from 'x' to directly attached neighbor 'v'

        (2) node x's distance vector, that is, [Dₓ(y) : y in N], containing x's estimate of its cost to all destinations, 'y' in 'N'

        (3) the distance vectors of EACH OF ITS NEIGHBORS, that is, Dᵥ = [Dᵥ(y) : y in N] for each neighbor 'v' of 'x'

    • In the distributed, asynchronous algorithm, from time to time, each node sends a copy of its distance vector to each of its neighbors:

        - when a node 'x' receives a new distance vector from any of its neighbors 'w', it saves w's distance vector, and then uses the Bellman-Ford equation to update its own distance vector
  
            dₓ(y) = minᵥ{c(x, v) + dᵥ(y)}   for each node 'y' in 'N'

        - if node x's distance vector has changed as a result of this update step, node 'x' will then send its updated distance vector to each of its neighbors, which can in turn update their own distance vectors . . .

        - in this way, as long as all the nodes continue to exchange their distance vectors in an asynchronous fashion, each cost estimate 'Dₓ' converges to 'dₓ(y)'

    • In summary, a node 'x' updates its distance-vector estimate when it either sees a cost change in one of its directly attached links or receives a distance-vector update from some neighbor.

        - importantly, to do this 'x' really needs to know the neighboring node v*(y) that is the next-hop router along the shortest path to 'y'; thus, for each destination node 'y', node 'x' also determines v*(y) and updates its forwarding table for destination 'y'

        - see figure 5.6 for an example of the distance-vector (DV) algorithm in operation; its pretty cool

### Distance-Vector Algorithm: Link-Cost Changes and Link Failure

    • Recall that when a node running the DV algorithm detects a change in the link cost from itself to a neighbor, it updates its distance vector, and if there’s a change in the cost of the least-cost path, informs its neighbors of its new distance vector.

        - see the discussion surrounding figure 5.7a, its pretty straighforward

    • When a link cost increases, interesting (and sometimes bad) things happen

        (1) assuming we are starting with Dᵧ(x) = 4, Dᵧ(z) = 1, and D𝓏(x) = 5

        (2) at time t₀, 'y' detects the link-cost change of Dᵧ(x) from 4 to 60.  'y' then computes  Dᵧ(x) = min{c(y, x) + Dₓ(x), c(y, z) + D𝓏(x)} = min{60 + 0, 1 + 5} = 6

        - of course, with this global view of the network (see figure 5.7b), this is clearly wrong, but the only information node 'y' has is that its direct cost to 'x' is 60 and that node 'z' has LAST told 'y' that 'z' could get to 'x' with a cost of 5

        (3) since node 'y' has computed a new minimum cost to 'x' (6), it informs 'z' of its new distance vector

        (4) at t₂, 'z' recieves this information from 'y' and comptues D𝓏(x) = min{50 + 0, 1 + 6} = 7 and since z's least cost to 'x' has increased, it then informs 'y' of its new distance vector

        (5) at t₃, after recieving z's new distance vector , 'y' determines Dᵧ(x) = min{60 + 0, 1 + 7} = 8, and the process continues for 44 iterations


    • The above was an exmaple of a routing loop; in order to get to 'x', 'y' routes through 'z', and 'z' routes through 'y' for several or an indefinite amount of iterations; the result of the bad news about the increase in link cost has indeed traveled slowly!

        - because of such scenarios, the problem we have seen is sometimes referred to as the count-to-infinity problem