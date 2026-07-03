#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#include "graph_types.h"

using namespace std;

bool readEdges(vector<edge> &edges, unordered_map<int, double> &delays, bool &negetive_edge, string file_path);

bool readNodes(set<int> &nodes, unordered_map<int, double> &delays, string file_path);
