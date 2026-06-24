#pragma once

#include <vector>
#include "graph_types.h"

void getFromDb(
    std::vector<int> &nodes,
    std::vector<double> &delays,
    std::vector<edge> &edges);

void createDb();