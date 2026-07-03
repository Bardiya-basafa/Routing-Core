
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <unordered_map>
#include "graph_types.h"

using namespace std;

bool readEdges(vector<edge> &edges, unordered_map<int, double> &delays, bool &negetive_edge, string file_path)
{
    ifstream file(file_path);

    if (!file.is_open())
    {
        return false;
    }

    string line;
    getline(file, line);
    while (getline(file, line))
    {
        stringstream ss(line);
        string sourceStr, targetStr, distanceStr, traficStr, weatherStr;
        getline(ss, sourceStr, ',');
        getline(ss, targetStr, ',');
        getline(ss, distanceStr, ',');
        getline(ss, traficStr, ',');
        getline(ss, weatherStr, ',');
        int u = stoi(sourceStr), v = stoi(targetStr);
        double distance = stod(distanceStr), trafic = stod(traficStr), weather = stod(weatherStr);
        if (distance * trafic * weather + delays[u] < 0 || distance * trafic * weather + delays[v] < 0)
        {
            negetive_edge = true;
        }

        edge e = {u, v, distance, trafic, weather};
        edge erev = {v, u, distance, trafic, weather};
        edges.push_back(e);
        edges.push_back(erev);
    }
    file.close();

    return true;
}

bool readNodes(set<int> &nodes, unordered_map<int, double> &delays, string file_path)
{

    ifstream file(file_path);

    if (!file.is_open())
    {
        return false;
    }

    string line;
    getline(file, line);
    while (getline(file, line))
    {
        stringstream ss(line);
        string nodeStr, delayStr;
        getline(ss, nodeStr, ',');
        getline(ss, delayStr, ',');
        int node = stoi(nodeStr);
        nodes.insert(node);
        delays[node] = stod(delayStr);
    }
    file.close();
    return true;
}
