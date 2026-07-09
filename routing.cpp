#include <map>
#include <vector>
#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <limits>
#include <set>
#include "file.h"
#include "graph_types.h"
#include "routing.h"

using namespace std;
using pii = pair<int, int>;
using pdi = pair<double, int>;
using pbs = pair<bool, string>;

Routing::Routing()
{
    INF = numeric_limits<double>::infinity();
    negetive_cycle = false;
    negetive_edge = false;
    V = 0;
};

map<int, double> Routing::dijkstra(int s)
{

    map<int, double> dist;
    for (auto &&n : nodes)
    {
        dist[n] = INF;
        parent[n] = -1;
    }

    dist[s] = 0;
    priority_queue<pdi, vector<pdi>, greater<pdi>> pq;
    pq.emplace(make_pair(0.0, s));
    while (!pq.empty())
    {
        pdi p = pq.top();
        pq.pop();
        int u = p.second;
        double d = p.first;
        if (d > dist[u])
        {
            continue;
        }

        for (auto &&e : graph[u])
        {
            int v = e.v;
            double w = e.total_cost() + delays[v];

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

map<int, double> Routing::bellmanFord(int s)
{

    map<int, double> dist;
    for (auto &&n : nodes)
    {
        dist[n] = INF;
        parent[n] = -1;
    }

    dist[s] = 0;

    for (int i = 0; i < V - 1; i++)
    {
        bool updated = false;
        for (auto &&item : graph)
        {
            for (auto &&e : item.second)
            {
                int u = e.u;
                int v = e.v;
                double w = e.total_cost() + delays[v];

                if (dist[u] != INF && dist[u] + w < dist[v])
                {
                    dist[v] = dist[u] + w;
                    parent[v] = u;
                    updated = true;
                }
            }
        }

        if (!updated)
            break;
    }

    // negative cycle check
    for (auto &&item : graph)
    {
        for (auto &&e : item.second)
        {
            int u = e.u;
            int v = e.v;
            double w = e.total_cost() + delays[v];

            if (dist[u] != INF && dist[u] + w < dist[v])
            {
                parent.clear();
                return {{-1, -1}};
            }
        }
    }
    return dist;
}

double Routing::shortestPathCost(int s, int t)
{
    if (s == t)
    {
        return 0;
    }

    map<int, double> dist;

    if (negetive_edge)
    {
        dist = bellmanFord(s);
    }
    else
    {
        dist = dijkstra(s);
    }

    if (dist.count(-1) && dist.at(-1) == -1)
    {
        return INF;
    }

    if (!dist.count(t))
    {
        return INF;
    }

    return dist.at(t);
}

pair<vector<int>, double> Routing::nearestNeighbor(int s, vector<int> destinations)
{

    vector<int> route;
    route.push_back(s);
    int current_node = s;
    unordered_set<int> unvisited(destinations.begin(), destinations.end());
    double total_cost = 0;
    while (!unvisited.empty())
    {
        int best_node = -1;
        double best_cost = INF;
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

bool Routing::getNodes(string filepath)
{
    nodes.clear();
    delays.clear();
    auto res = readNodes(nodes, delays, filepath);
    return res;
}

bool Routing::getEdges(string filepath)
{
    for (auto &&u : nodes)
    {
        graph[u].clear();
    }

    negetive_edge = false;
    auto res = readEdges(graph, delays, negetive_edge, filepath);
    return res;
}

void Routing::initialize()
{
    parent.clear();
    negetive_cycle = false;
    V = nodes.size();

    parent.clear();

    map<int, double> dist = bellmanFord(*nodes.begin());
    if (dist[-1] == -1)
    {
        negetive_cycle = true;
    }
}
