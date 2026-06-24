#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <queue>
#include <set>
#include "graph_types.h"
#include "database.h"

using namespace std;
using pii = pair<int, int>;

vector<int> nodes;
vector<double> delays;
vector<edge> edges;
vector<vector<edge>> graph;
vector<int> parent;
int V;
bool negetive_cycle = false;
bool negetive_edge = false;

vector<double> dijstra(int s);

vector<double> bellmanFord(int s);

double shortestPathCost(int s, int t);

pair<vector<int>, double> nearestNeighbor(int s, vector<int> destinations);

void initialize();

int main(int argc, char const *argv[])
{
    // initialize data structures
    initialize();
    if (negetive_cycle)
    {
        cout << "Graph has negetive cycle and cant find any shortest path!";
        return 0;
    }

    // dijstra
    // auto dist = dijstra(0);
    // // bellman ford
    // auto dist1 = bellmanFord(0);

    auto [route, total_cost] = nearestNeighbor(0, {1, 2, 3, 4});

    cout << total_cost << endl;
    for (auto &&n : route)
    {
        cout << n << " ";
    }

    return 0;
}

vector<double> dijstra(int s)
{
    vector<double> dist(V, __INT_MAX__);
    dist[s] = 0;
    priority_queue<pii, vector<pii>, greater<pii>> pq;
    pq.emplace(make_pair(0, s));
    while (!pq.empty())
    {
        pii p = pq.top();
        pq.pop();
        int u = p.second;
        int d = p.first;
        if (d > dist[u])
        {
            continue;
        }

        for (auto &&e : graph[u])
        {
            int v = e.v;
            int w = e.total_cost() + delays[v];

            if (dist[u] + w < dist[v])
            {
                parent[v] = u;
                dist[v] = dist[u] + w;
                pq.emplace(make_pair(dist[v], v));
            }
        }
    }

    return dist;
}

vector<double> bellmanFord(int s)
{

    vector<double> dist(V, __INT_MAX__);
    dist[s] = 0;

    for (int i = 0; i < V; i++)
    {
        for (auto &&e : edges)
        {
            int u = e.u;
            int v = e.v;
            int w = e.total_cost() + delays[v];
            if (dist[u] + w < dist[v])
            {

                if (i == V - 1)
                {
                    return {-1};
                    parent.clear();
                }
                dist[v] = dist[u] + w;
                parent[v] = u;
            }
        }
    }

    return dist;
}

double shortestPathCost(int s, int t)
{
    if (s == t)
    {
        return 0;
    }

    double cost = 0;
    if (negetive_edge)
    {
        vector<double> dist = bellmanFord(s);
        cost = dist[t];
    }
    else
    {
        vector<double> dist = dijstra(s);
        cost = dist[t];
    }

    return cost;
}

pair<vector<int>, double> nearestNeighbor(int s, vector<int> destinations)
{

    vector<int> route;
    route.push_back(s);
    int current_node = s;
    set<int> unvisited(destinations.begin(), destinations.end());
    double total_cost = 0;
    while (!unvisited.empty())
    {
        int best_node = -1;
        int best_cost = __INT_MAX__;
        for (auto &&n : unvisited)
        {
            double path_cost = shortestPathCost(current_node, n);
            if (path_cost < best_cost)
            {
                best_cost = path_cost;
                best_node = n;
            }
        }

        route.push_back(best_node);
        total_cost += best_cost;
        current_node = best_node;
        unvisited.erase(best_node);
    }

    total_cost += shortestPathCost(current_node, s);
    route.push_back(s);
    return make_pair(route, total_cost);
}

void initialize()
{

    createDb();
    // getFromDb(nodes, delays, edges);

    // nodes = {0, 1, 2, 3, 4};
    // V = nodes.size();
    // delays = {0, 0, 0, 0, 0};
    // edges = {
    //     {0, 1, 4, 0, 0},
    //     {0, 2, 8, 0, 0},
    //     {1, 2, 3, 0, 0},
    //     {1, 4, 6, 0, 0},
    //     {4, 3, 10, 0, 0},
    //     {2, 3, 2, 0, 0},
    // };

    // nodes = {0, 1, 2, 3, 4, 5};
    // V = nodes.size();

    // delays = {0, 0, 0, 0, 0, 0};

    // edges = {
    //     {0, 1, 10, 0, 0},
    //     {0, 2, 15, 0, 0},
    //     {0, 3, 20, 0, 0},
    //     {0, 4, 25, 0, 0},
    //     {0, 5, 18, 0, 0},

    //     {1, 2, 35, 0, 0},
    //     {1, 3, 25, 0, 0},
    //     {1, 4, 17, 0, 0},
    //     {1, 5, 28, 0, 0},

    //     {2, 3, 30, 0, 0},
    //     {2, 4, 20, 0, 0},
    //     {2, 5, 12, 0, 0},

    //     {3, 4, 15, 0, 0},
    //     {3, 5, 22, 0, 0},

    //     {4, 5, 16, 0, 0},
    // };
    nodes = {0, 1, 2, 3, 4};
    V = nodes.size();

    delays = {0, 0, 0, 0, 0};

    edges = {
        {0, 1, 29, 0, 0},
        {0, 2, 20, 0, 0},
        {0, 3, 21, 0, 0},
        {0, 4, 16, 0, 0},

        {1, 2, 15, 0, 0},
        {1, 3, 29, 0, 0},
        {1, 4, 28, 0, 0},

        {2, 3, 15, 0, 0},
        {2, 4, 14, 0, 0},

        {3, 4, 4, 0, 0},
    };

    graph.resize(V);
    parent.resize(V);

    // complete graph

    for (auto &&u : nodes)
    {
        for (auto &&edg : edges)
        {
            if (edg.u == u)
            {
                graph[u].push_back(edg);
            }
            else if (edg.v == u)
            {
                edge new_edg = edg;
                swap(new_edg.u, new_edg.v);
                graph[u].push_back(new_edg);
            }
        }
    }

    vector<double> dist = bellmanFord(0);
    if (dist[0] == -1)
    {
        negetive_cycle = true;
    }

    for (auto &&e : edges)
    {
        if (e.total_cost() < 0)
        {
            negetive_edge = true;
            return;
        }
    }
}
