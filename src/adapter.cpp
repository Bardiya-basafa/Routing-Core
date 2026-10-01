// Adapter: wraps the Routing engine and outputs JSON for the web UI.
// Does NOT modify any existing source — just links against routing.cpp + file.cpp.
//
// Usage:
//   adapter graph  <nodes.csv> <edges.csv>
//   adapter path   <nodes.csv> <edges.csv> <source> <target>
//   adapter all    <nodes.csv> <edges.csv> <source>
//   adapter tsp    <nodes.csv> <edges.csv> <start> <dest1,dest2,...>

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <sstream>
#include <limits>
#include "routing.h"

using namespace std;

// ---- JSON helpers --------------------------------------------------

static string numStr(double d)
{
    if (d == numeric_limits<double>::infinity()) return "null";
    ostringstream ss;
    ss << d;
    return ss.str();
}

static string nodeList(const set<int> &nodes)
{
    string s = "[";
    bool first = true;
    for (int n : nodes) { if (!first) s += ","; s += to_string(n); first = false; }
    return s + "]";
}

static string pathStr(const vector<int> &path)
{
    string s = "[";
    for (size_t i = 0; i < path.size(); i++) { if (i) s += ","; s += to_string(path[i]); }
    return s + "]";
}

static string distMap(const map<int, double> &dist)
{
    string s = "{";
    bool first = true;
    for (const auto &[n, d] : dist) { if (!first) s += ","; s += "\"" + to_string(n) + "\":" + numStr(d); first = false; }
    return s + "}";
}

static string parentMap(const map<int, int> &parent)
{
    string s = "{";
    bool first = true;
    for (const auto &[v, u] : parent) { if (!first) s += ","; s += "\"" + to_string(v) + "\":" + to_string(u); first = false; }
    return s + "}";
}

static string delayMap(const unordered_map<int, double> &delays)
{
    string s = "{";
    bool first = true;
    for (const auto &[n, d] : delays) { if (!first) s += ","; s += "\"" + to_string(n) + "\":" + numStr(d); first = false; }
    return s + "}";
}

// ---- Main ----------------------------------------------------------

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        cout << "{\"error\":\"Usage: adapter <command> <nodes_file> <edges_file> [args...]\"}" << endl;
        return 1;
    }

    string cmd       = argv[1];
    string nodesFile = argv[2];
    string edgesFile = argv[3];

    Routing r;
    if (!r.getNodes(nodesFile))
    {
        cout << "{\"error\":\"Could not load nodes file: " << nodesFile << "\"}" << endl;
        return 0;
    }
    if (!r.getEdges(edgesFile))
    {
        cout << "{\"error\":\"Could not load edges file: " << edgesFile << "\"}" << endl;
        return 0;
    }
    r.initialize();

    int ec = 0;
    for (const auto &[_, nbrs] : r.getGraph())
        ec += static_cast<int>(nbrs.size());
    ec /= 2;

    // ---- graph ----
    if (cmd == "graph")
    {
        stringstream ss;
        ss << "{\"nodes\":" << nodeList(r.getNodes());
        ss << ",\"delays\":" << delayMap(r.getDelays());
        ss << ",\"edgeCount\":" << ec;
        ss << ",\"hasNegativeEdge\":" << (r.hasNegativeEdge() ? "true" : "false");
        ss << ",\"hasNegativeCycle\":" << (r.hasNegativeCycle() ? "true" : "false");
        ss << ",\"edges\":[";
        set<string> seen;
        bool first = true;
        for (const auto &[u, nbrs] : r.getGraph())
        {
            for (const auto &e : nbrs)
            {
                string key = to_string(min(e.u, e.v)) + "-" + to_string(max(e.u, e.v));
                if (seen.count(key)) continue;
                seen.insert(key);
                if (!first) ss << ",";
                ss << "{\"u\":" << e.u << ",\"v\":" << e.v;
                ss << ",\"distance\":" << numStr(e.d);
                ss << ",\"traffic\":" << numStr(e.t);
                ss << ",\"weather\":" << numStr(e.w);
                ss << ",\"cost\":" << numStr(e.total_cost());
                ss << "}";
                first = false;
            }
        }
        ss << "]}";
        cout << ss.str() << endl;
    }
    // ---- path ----
    else if (cmd == "path")
    {
        if (argc < 6) { cout << "{\"error\":\"Missing source/target\"}" << endl; return 1; }
        int src = stoi(argv[4]), tgt = stoi(argv[5]);
        if (r.hasNegativeCycle())
        {
            cout << "{\"error\":\"Graph contains a negative cycle; shortest paths are undefined\"}" << endl;
            return 0;
        }
        map<int, double> dist = r.hasNegativeEdge() ? r.bellmanFord(src) : r.dijkstra(src);
        vector<int> path = r.reconstructPath(tgt);
        double cost = numeric_limits<double>::infinity();
        auto it = dist.find(tgt);
        if (it != dist.end()) cost = it->second;
        bool unreachable = (cost == numeric_limits<double>::infinity());
        cout << "{\"source\":" << src << ",\"target\":" << tgt;
        cout << ",\"distance\":" << numStr(cost);
        cout << ",\"path\":" << pathStr(path);
        cout << ",\"unreachable\":" << (unreachable ? "true" : "false");
        cout << "}" << endl;
    }
    // ---- all ----
    else if (cmd == "all")
    {
        if (argc < 5) { cout << "{\"error\":\"Missing source\"}" << endl; return 1; }
        int src = stoi(argv[4]);
        if (r.hasNegativeCycle())
        {
            cout << "{\"error\":\"Graph contains a negative cycle; shortest paths are undefined\"}" << endl;
            return 0;
        }
        map<int, double> dist = r.hasNegativeEdge() ? r.bellmanFord(src) : r.dijkstra(src);
        cout << "{\"source\":" << src;
        cout << ",\"distances\":" << distMap(dist);
        cout << ",\"parents\":" << parentMap(r.getParent());
        cout << "}" << endl;
    }
    // ---- tsp ----
    else if (cmd == "tsp")
    {
        if (argc < 6) { cout << "{\"error\":\"Missing start/destinations\"}" << endl; return 1; }
        int start = stoi(argv[4]);
        vector<int> dests;
        stringstream ss(argv[5]);
        string tok;
        while (getline(ss, tok, ','))
            if (!tok.empty()) dests.push_back(stoi(tok));
        if (r.hasNegativeCycle())
        {
            cout << "{\"error\":\"Graph contains a negative cycle; shortest paths are undefined\"}" << endl;
            return 0;
        }
        auto [route, cost] = r.nearestNeighbor(start, dests);
        cout << "{\"start\":" << start;
        cout << ",\"route\":" << pathStr(route);
        cout << ",\"cost\":" << numStr(cost);
        cout << "}" << endl;
    }
    else
    {
        cout << "{\"error\":\"Unknown command: " << cmd << "\"}" << endl;
        return 1;
    }
    return 0;
}
