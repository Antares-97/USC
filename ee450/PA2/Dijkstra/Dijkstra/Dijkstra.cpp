//
//  Dijkstra.cpp
//  Dijkstra
//
//  Created by LJZ's macbook pro on 11/13/19.
//  Copyright © 2019 LJZ's macbook pro. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <vector>
#include "string.h"
#include <string>
#include <utility>
#include <queue>
#include <algorithm>
#include <stack>
#define INF 0xffffff
using namespace std;


typedef struct E                        // struct type: edge
{
    int begin;
    int end;
    int weight;                         // A directed edge <u, v> has 3 properties: two vertexs and an edge weight
    E(int b, pair<int, int> p)
    {
        begin = b;
        end = p.first;
        weight = p.second;
    }
    E(int b, int e, int w)              // constructors
    {
        begin = b;
        end = e;
        weight = w;
    }
    bool operator < (const E b) const
    {
        return this->weight < b.weight;
    }
    bool operator > (const E b) const
    {
        return this->weight > b.weight;
    }
    friend ostream & operator << (ostream &out, const E & e)
    {
        out << e.begin << " "<< e.end << " " << e.weight;
        return out;
    }
}edge;


vector<pair<int, int>> * buildGraph(int n)                  // read from file txt, and build an Adjacency List
{
    vector<pair<int, int>> * graph;                         // indices represent the begin node u
    graph = new vector<pair<int, int>> [n];                 // graph[i] is a vector, consisting of pairs, each pair represent an edge, the first number is the end point, the second number is the edge weight.
    
    int begin, end, weight;
    while(cin>>begin>>end>>weight)
    {
        pair<int, int> Edge = make_pair(end, weight);
        graph[begin].push_back(Edge);
        Edge = make_pair(begin, weight);
        graph[end].push_back(Edge);
    }
    return graph;
}


pair<int, int> * Dijkstra(int n, vector<pair<int, int>> * graph)
{
    int visited[n];
    memset(visited, 0, sizeof(visited));                    // all points not visited
    pair<int, int> * result;                                // The array stores the shortest distance to all nodes
    result = new pair<int, int>[n];
    for (int i=1; i<n; i++)
    {
        result[i].first = INF;                              // distance to other nodes set to inf
        result[i].second = -1;                              // previous recursor, last node in the path, to recover path
    }
    visited[0] = 1;                                         // node 0 is the source, mark it as visited.
    result[0].first = 0;                                    // from source to itself, the distance is 0.
    result[0].second = 0;                                   // the previous node is itself.
    
    priority_queue<edge, vector<edge>, greater<edge>> pq;   // priority queue, a min-heap, used for maintaining the shortest path
    for(int i=0; i<graph[0].size(); i++)
        pq.push(edge(0, graph[0][i]));                      // starting from source, push all its edge into the queue
    
    while (!pq.empty())                                     // When the queue is empty, Dijkstra algorithm ends
    {
        edge e = pq.top();                                  // the edge on the top of the queue has the shortest distance going from the source
        pq.pop();
        if(visited[e.end] == 1)                             // if the node e.end is already visited, ignore it
            continue;

        visited[e.end] = 1;                                 // e.end is the newly added point
        result[e.end].first = e.weight;                     // weight has already renewed as dv + le, the shortest distance
        result[e.end].second = e.begin;                     // the previous point makes it shortest
        
        for (int i=0; i<graph[e.end].size(); i++)           // renew the adjacent nodes' distance using the newly added node
        {
            if(visited[graph[e.end][i].first] == 1)
                continue;
            result[graph[e.end][i].first].first =
                 min(result[graph[e.end][i].first].first, e.weight + graph[e.end][i].second);       // d[v] = min(d[v], d[u] + edge[u][v])
            pq.push(edge(e.end, graph[e.end][i].first, result[graph[e.end][i].first].first));       // push all its adjacent edges into the priority queue
        }
    }
    return result;
}


int * recover_path(int n, pair<int, int> * result)          // Given the result returned from Dijkstra, recover each path using the previous recursor
{
    printf("Minimum path tree (represented in path):\n");
    int * next_hop = new int[n];
    
    for (int i=1; i<n; i++)
    {
        stack<int> st;                                      // We use a stack to store every node we traverse, and pour it out to recover the path
        st.push(i);
        
        int k = i;
        while(result[k].second != 0)
        {
            k = result[k].second;                           // Go back through its recursor
            st.push(k);
        }
        next_hop[i] = k;                                    // k equals the next hop of the path starting from the source
        
        printf("0 ");
        while (!st.empty())
        {
            printf("%d ", st.top());                        // The order in stack is exactly the reverse order of the path
            st.pop();
        }
        printf("\n");
    }
    
    return next_hop;
}


int main(int argc, const char * argv[])
{
    freopen("./Adj.txt", "r", stdin);
    int n;                                                  // number of nodes
    scanf("%d", &n);
    
    vector<pair<int, int>> * graph = buildGraph(n);         // Build the graph
    pair<int, int> * result = Dijkstra(n, graph);           // Run Dijkstra
    int * next_hop = recover_path(n, result);               // Recover the path
    
    printf("Routing table (<Destination> <Cost> <Next hop>):\n");
    for (int i=1; i<n; i++)
        printf("%d %d %d\n", i, result[i].first, next_hop[i]);
    return 0;
}
