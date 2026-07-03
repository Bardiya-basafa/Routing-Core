#ifndef ROUTING_H
#define ROUTING_H

#include <map>
#include <vector>
#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <limits>
#include <set>
#include "file.h"
#include "graph_types.h"

using namespace std;

class Routing
{

public:
    set<int> nodes;
    unordered_map<int, double> delays;
    vector<edge> edges;
    map<int, vector<edge>> graph;
    map<int, int> parent;

    int V;
    bool negetive_cycle;
    bool negetive_edge;
    double INF;

    Routing();

    map<int, double> dijkstra(int s);

    map<int, double> bellmanFord(int s);

    double shortestPathCost(int s, int t);

    pair<vector<int>, double> nearestNeighbor(int s, vector<int> destinations);

    bool getNodes(string filepath);

    bool getEdges(string filepath);

    void initialize();
};

#endif
