# EE450 - Programming Assignment 2
#### Junzhe Liu / 2270250947
#### Nov 14th

In this assignment, we implemented the classic routing algorithm: Dijkstra Algorithm using priority queue.

## Dijkstra Algorithm

Dijkstra Algorithm is a classic algorithm for solving single source shortest path in a graph with positive edges. Let the node at which we are starting be called the initial node. Let the distance of node Y be the distance from the initial node to Y. Dijkstra's algorithm will assign some initial distance values and will try to improve them step by step.

1.Mark all nodes unvisited. Create a set of all the unvisited nodes called the unvisited set.
2.Assign to every node a tentative distance value: set it to zero for our initial node and to infinity for all other nodes. Set the initial node as current.
3.For the current node, consider all of its unvisited neighbours and calculate their tentative distances through the current node. Compare the newly calculated tentative distance to the current assigned value and assign the smaller one. 
4.When we are done considering all of the unvisited neighbours of the current node, mark the current node as visited and remove it from the unvisited set. A visited node will never be checked again.
5.If the destination node has been marked visited (when planning a route between two specific nodes) or if the smallest tentative distance among the nodes in the unvisited set is infinity (when planning a complete traversal; occurs when there is no connection between the initial node and remaining unvisited nodes), then stop. The algorithm has finished.
6.Otherwise, select the unvisited node that is marked with the smallest tentative distance, set it as the new "current node", and go back to step 3.

When planning a route, it is actually not necessary to wait until the destination node is "visited" as above: the algorithm can stop once the destination node has the smallest tentative distance among all "unvisited" nodes (and thus could be selected as the next "current").

In implementation, we used priority queue to facilitate the step of finding the shortest reachable new node. In psuedocode:
```cpp
function Dijkstra(Graph, source):
    create vertex set visit
    for each vertex v in Graph:             
        dist[v] ← INFINITY                  
        prev[v] ← UNDEFINED    
        visit[v] ← FALSE
    dist[source] ← 0
    visit[source] ← TRUE
    
    create Priority Queue <edge> pq            // a min-heap, maintains all the edges going from the set S consists of visited points.
    Put all edges going out from the source point into pq

    while pq is not empty:
        edge e ← top element of pq, edge with the min dist[u]
        u ← end point of edge e, next added point into the set S
        remove e from pq
        if u is not visited :
            visit[u] = TRUE
            for each neighbor v of u and v is not visited:
                alt ← dist[u] + length(u, v)
                if alt < dist[v]:               
                    dist[v] ← alt 
                    prev[v] ← u 
                push edge(u,v,dist[v]) into pq
    return dist[], prev[]
```

With the Priority Queue, the Dijkstra Algorithm's complexity can be reduced to $$O(|E|\log|V|)$$.



## Contents of code

#### 1. Graph: Adjacent List
We use adjacent list to store the information of the graph, but the adjacent graph utilized in our algorithm is more compact than the original input. Here I used An array to store the graph: 
```cpp
vector<pair<int, int>> * graph;
graph = new vector<pair<int, int>> [n]; 
```
each entry $$graph[i]$$ is a vector, it stores all the edges going out from node $$i$$, the element type of vector $$graph[i]$$ is a pair, a pair has two integers, the first number is the end point $$v$$ of this edge, the second number $$w$$ is the weight of this edge.

For example, if node 2 has two edges going out from it, i.e. <2,3> with weight 1, and <2,5> with a weight of 4. Then $$graph[2]$$ is a vector of size 2, $$graph[2][0]$$ = <3,1> and  $$graph[2][1]$$ = <5,2>.

With this data structure, we can quickly locate the adjacent nodes of a given node, and its corresponding out edges, without traverse the whole graph.


#### 2. edge: struct E

edge is a struct type, it has 3 elements: begin, end and weight, representing the beginning vertex, end vertex and the edge weight respectively. It has two constructors: either construct an edge using 3 numbers or an integer with an integer pair.

More importantly, in order to compare two edges and make it possible for the priority queue to pop out the edge with the smallest weight, I overload the operator "<" and ">", and we compare two edges by their weights:
```cpp
typedef struct E                        // struct type: edge
{
    /* elements and constructer definition */
    bool operator < (const E b) const
        return this->weight < b.weight;
    bool operator > (const E b) const
        return this->weight > b.weight;
}edge;
```

#### 3. pq: priority_queue<edge, vector<edge>, greater<edge>>

priority queue is adopted in the Dijkstra Algorithm. It maintains all the edges going out from our already visited point set S, and in every iteration, it pops out an edge such that the end point $$v\notin S$$ and has the shortest path length from the source compared to other unvisited points.

Its element has the type of previous defined $$edge$$. We use the parameter ***greater<edge>*** to construct a min-heap, so the priority queue will automatically sort the edges in the order of increasing weights:
```cpp
priority_queue<edge, vector<edge>, greater<edge>> pq;
```

#### 4. result: pair<int, int> * 

This is the return value of the Dijkstra Algorithm. It's actually an array, it stores the shortest path length to every nodes and the last point in the path before reaching the destination:
```cpp
result = new pair<int, int>[n];
for (int i=1; i<n; i++)
{
    result[i].first = INF;     // distance to other nodes set to inf
    result[i].second = -1;     // previous recursor, last node in the path, used to recover path
}
```


#### 5. next_hop: int *

In the algorithm ***recover_path***, we use the recursor which restored in the *result[i].second* to recover the path, this is like a pointer pointing to the previous node in the path, and by moving to the previous point we can build up the path recursively. While moving along the path, we'll eventually end up on some point *u* where *result[u].second == 0*, this means *u* is the next hop in the path from *s* to *i*. We store this point *u* in **nexthop[i]**.


#### 6. makefile

makefile is used to compile and run the above cpp programs:
```bash
# /* Makefile */
# /* Makefile */
all: Dijkstra.cpp
	g++ -o Dijkstra.out Dijkstra.cpp
	./Dijkstra.out

clean:
	-rm Dijkstra.out
```

Please use "make all" or "make" first to compile and run the algorithm. If you need to re-compile, use "make clean" to delete all executable files.


## Idiosyncrasy

The node number should always be in the range of $$[0, n-1]$$, otherwise the program will not give out the correct answer. The source point is always labelled as $$0$$, and the edge weights should never be negative. Furthermore, if a node is not reachable from the source, the distance will be $$16777215$$.

## Reused Code

All the codes are written by myself.
