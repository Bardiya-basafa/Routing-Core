
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <unordered_map>
#include "graph_types.h"

using namespace std;

bool readEdges(map<int, vector<edge>> &graph, unordered_map<int, double> &delays, bool &negetive_edge, string file_path)
{
    ifstream file(file_path);

    if (!file.is_open())
    {
        return false;
    }

    try
    {
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
            graph[u].push_back(e);
            graph[v].push_back(erev);
        }
    }
    catch (const std::exception &e)
    {
        cout << "File format is incorrect" << endl;
        graph.clear();
        negetive_edge = false;
        return false;
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

    try
    {
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
    }
    catch (const std::exception &e)
    {
        cout << "File format is incorrect" << endl;
        nodes.clear();
        delays.clear();
        return false;
    }

    file.close();
    return true;
}
