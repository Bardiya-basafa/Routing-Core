#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#include <map>
#include "graph_types.h"

using namespace std;

bool readEdges(map<int, vector<edge>> &graph, unordered_map<int, double> &delays, bool &negetive_edge, string file_path);

bool readNodes(set<int> &nodes, unordered_map<int, double> &delays, string file_path);
