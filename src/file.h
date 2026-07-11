#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#include <map>
#include "graph_types.h"

/**
 * @brief Reads an edge CSV and fills the adjacency list.
 *
 * The expected CSV format:
 *   source,target,distance,traffic,weather
 *
 * @param graph            (out) Adjacency list: node → outgoing edges.
 * @param delays           (in)  Map of node → delay (used to detect negative total weights).
 * @param has_negative_edge (out) True if any edge weight (edge + destination delay) is negative.
 * @param file_path         Path to the CSV file.
 * @return True on success, false on error.
 */
bool readEdges(std::map<int, std::vector<edge>> &graph,
               const std::unordered_map<int, double> &delays,
               bool &has_negative_edge,
               const std::string &file_path);

/**
 * @brief Reads a node CSV and populates the set of nodes and their delays.
 *
 * The expected CSV format:
 *   node,delay
 *
 * @param nodes    (out) Set of all node IDs.
 * @param delays   (out) Map node ID → delay value.
 * @param file_path  Path to the CSV file.
 * @return True on success, false on error.
 */
bool readNodes(std::set<int> &nodes,
               std::unordered_map<int, double> &delays,
               const std::string &file_path);