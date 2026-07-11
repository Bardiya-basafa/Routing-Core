#include "routing.h"
#include <limits>
#include <queue>
#include <algorithm> // for reverse
#include <stdexcept>

using namespace std;
using pdi = pair<double, int>;

Routing::Routing()
{
    INF_ = numeric_limits<double>::infinity();
    has_negative_cycle_ = false;
    has_negative_edge_ = false;
    V_ = 0;
}

map<int, double> Routing::dijkstra(int s)
{
    last_source_ = s;
    map<int, double> dist;
    for (int n : nodes_)
    {
        dist[n] = INF_;
        parent_[n] = -1;
    }
    dist[s] = 0.0;
    priority_queue<pdi, vector<pdi>, greater<pdi>> pq;
    pq.emplace(0.0, s);

    while (!pq.empty())
    {
        auto [d, u] = pq.top();
        pq.pop();
        if (d > dist[u])
            continue;

        for (const auto &e : graph_[u])
        {
            int v = e.v;
            double w = e.total_cost() + delays_[v];
            if (dist[u] + w < dist[v])
            {
                parent_[v] = u;
                dist[v] = dist[u] + w;
                pq.emplace(dist[v], v);
            }
        }
    }
    return dist;
}

map<int, double> Routing::bellmanFord(int s)
{
    last_source_ = s;
    map<int, double> dist;
    for (int n : nodes_)
    {
        dist[n] = INF_;
        parent_[n] = -1;
    }
    dist[s] = 0.0;

    // Relax edges V-1 times
    for (int i = 0; i < V_ - 1; ++i)
    {
        bool updated = false;
        for (const auto &[u, neighbors] : graph_)
        {
            for (const auto &e : neighbors)
            {
                int v = e.v;
                double w = e.total_cost() + delays_[v];
                if (dist[u] != INF_ && dist[u] + w < dist[v])
                {
                    dist[v] = dist[u] + w;
                    parent_[v] = u;
                    updated = true;
                }
            }
        }
        if (!updated)
            break;
    }

    // Detect negative cycles
    for (const auto &[u, neighbors] : graph_)
    {
        for (const auto &e : neighbors)
        {
            int v = e.v;
            double w = e.total_cost() + delays_[v];
            if (dist[u] != INF_ && dist[u] + w < dist[v])
            {
                parent_.clear();
                return {}; // empty map = negative cycle detected
            }
        }
    }
    return dist;
}

double Routing::shortestPathCost(int s, int t)
{
    if (s == t)
        return 0.0;

    map<int, double> dist;
    if (has_negative_edge_)
    {
        dist = bellmanFord(s);
    }
    else
    {
        dist = dijkstra(s);
    }

    // Negative cycle detection
    if (dist.empty())
        return INF_;

    auto it = dist.find(t);
    return (it != dist.end()) ? it->second : INF_;
}

vector<int> Routing::reconstructPath(int dest) const
{
    vector<int> path;
    if (parent_.count(dest) == 0)
        return path; // dest not in graph

    if (dest == last_source_)
    {
        return {dest};
    }

    // If the node is unreachable, its parent remains -1
    if (parent_.at(dest) == -1)
    {
        return {}; // unreachable
    }

    int cur = dest;
    while (cur != -1)
    {
        path.push_back(cur);
        cur = parent_.at(cur);
    }
    reverse(path.begin(), path.end());
    return path;
}

pair<vector<int>, double> Routing::nearestNeighbor(int start,
                                                   const vector<int> &destinations)
{
    vector<int> route = {start};
    int current = start;
    unordered_set<int> unvisited(destinations.begin(), destinations.end());
    double total_cost = 0.0;

    while (!unvisited.empty())
    {
        int best_node = -1;
        double best_cost = INF_;
        for (int n : unvisited)
        {
            double cost = shortestPathCost(current, n);
            if (cost < best_cost)
            {
                best_cost = cost;
                best_node = n;
            }
        }
        route.push_back(best_node);
        total_cost += best_cost;
        current = best_node;
        unvisited.erase(best_node);
    }

    total_cost += shortestPathCost(current, start);
    route.push_back(start);
    return {route, total_cost};
}

bool Routing::getNodes(const string &filepath)
{
    nodes_.clear();
    delays_.clear();
    return readNodes(nodes_, delays_, filepath);
}

bool Routing::getEdges(const string &filepath)
{
    for (int u : nodes_)
        graph_[u].clear();
    has_negative_edge_ = false;
    return readEdges(graph_, delays_, has_negative_edge_, filepath);
}

void Routing::initialize()
{
    parent_.clear();
    has_negative_cycle_ = false;
    V_ = static_cast<int>(nodes_.size());
    if (V_ == 0)
        return;

    // Run Bellman‑Ford from an arbitrary node to detect negative cycles
    map<int, double> test = bellmanFord(*nodes_.begin());
    if (test.empty())
    {
        has_negative_cycle_ = true;
    }
}