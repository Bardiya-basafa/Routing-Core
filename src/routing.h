#pragma once

#include <map>
#include <vector>
#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <limits>
#include <set>
#include <string>
#include "file.h"
#include "graph_types.h"

/**
 * @brief Core routing engine offering shortest‑path and TSP‑like queries.
 *
 * The graph is undirected. Edge weight = edge.total_cost() + delay[target].
 * Supports Dijkstra (non‑negative edges) and Bellman‑Ford (allows negative edges,
 * detects negative cycles).
 */
class Routing
{
public:
    Routing();

    // ---------- File I/O ----------
    bool getNodes(const std::string &filepath);
    bool getEdges(const std::string &filepath);

    /** Must be called after loading nodes & edges to detect negative cycles. */
    void initialize();

    // ---------- Shortest Path Algorithms ----------
    std::map<int, double> dijkstra(int source);
    std::map<int, double> bellmanFord(int source);

    /** Shortest path cost between two nodes. Returns +∞ if unreachable or negative cycle exists. */
    double shortestPathCost(int from, int to);

    /**
     * @brief Reconstruct the shortest path from the last Dijkstra/Bellman‑Ford source
     *        to the given destination.
     * @param dest Destination node ID.
     * @return Sequence of nodes from source to dest, empty if unreachable.
     */
    std::vector<int> reconstructPath(int dest) const;

    // ---------- TSP Heuristic ----------
    std::pair<std::vector<int>, double> nearestNeighbor(int start,
                                                        const std::vector<int> &destinations);

    // ---------- Accessors (for UI) ----------
    const std::set<int> &getNodes() const { return nodes_; }
    const std::map<int, std::vector<edge>> &getGraph() const { return graph_; }
    const std::map<int, int> &getParent() const { return parent_; }
    bool hasNegativeCycle() const { return has_negative_cycle_; }
    bool hasNegativeEdge() const { return has_negative_edge_; }

private:
    std::set<int> nodes_;
    std::unordered_map<int, double> delays_;
    std::map<int, std::vector<edge>> graph_;
    std::map<int, int> parent_;
    int V_;
    bool has_negative_cycle_;
    bool has_negative_edge_;
    double INF_;
    int last_source_ = -1; // source of last shortest-path run (for path reconstruction)
};