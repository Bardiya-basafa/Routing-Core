#include "file.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm> // for remove

using namespace std;

bool readEdges(map<int, vector<edge>> &graph,
               const unordered_map<int, double> &delays,
               bool &has_negative_edge,
               const string &file_path)
{
    ifstream file(file_path);
    if (!file.is_open())
        return false;

    // ---------- Validate header ----------
    string header;
    if (!getline(file, header))
    {
        cerr << "Error: Edges file is empty." << endl;
        return false;
    }
    header.erase(remove(header.begin(), header.end(), '\r'), header.end());
    if (header != "source,target,distance,traffic,weather")
    {
        cerr << "Error: Invalid header in edges file. Expected: source,target,distance,traffic,weather" << endl;
        return false;
    }

    try
    {
        string line;
        while (getline(file, line))
        {
            stringstream ss(line);
            string src_str, tgt_str, dist_str, traf_str, weath_str;
            getline(ss, src_str, ',');
            getline(ss, tgt_str, ',');
            getline(ss, dist_str, ',');
            getline(ss, traf_str, ',');
            getline(ss, weath_str, ',');

            int u = stoi(src_str), v = stoi(tgt_str);
            double distance = stod(dist_str);
            double traffic = stod(traf_str);
            double weather = stod(weath_str);

            edge forward = {u, v, distance, traffic, weather};
            edge backward = {v, u, distance, traffic, weather};

            // Negative weight detection: total cost including destination delay
            if (forward.total_cost() + delays.at(v) < 0.0 ||
                backward.total_cost() + delays.at(u) < 0.0)
            {
                has_negative_edge = true;
            }

            graph[u].push_back(forward);
            graph[v].push_back(backward);
        }
    }
    catch (const std::exception &e)
    {
        cerr << "Error: File format is incorrect (" << e.what() << ")" << endl;
        graph.clear();
        has_negative_edge = false;
        return false;
    }
    return true;
}

bool readNodes(set<int> &nodes, unordered_map<int, double> &delays,
               const string &file_path)
{
    ifstream file(file_path);
    if (!file.is_open())
        return false;

    // ---------- Validate header ----------
    string header;
    if (!getline(file, header))
    {
        cerr << "Error: Nodes file is empty." << endl;
        return false;
    }
    header.erase(remove(header.begin(), header.end(), '\r'), header.end());
    if (header != "node,delay")
    {
        cerr << "Error: Invalid header in nodes file. Expected: node,delay" << endl;
        return false;
    }

    try
    {
        string line;
        while (getline(file, line))
        {
            stringstream ss(line);
            string node_str, delay_str;
            getline(ss, node_str, ',');
            getline(ss, delay_str, ',');
            int node = stoi(node_str);
            nodes.insert(node);
            delays[node] = stod(delay_str);
        }
    }
    catch (const std::exception &e)
    {
        cerr << "Error: File format is incorrect (" << e.what() << ")" << endl;
        nodes.clear();
        delays.clear();
        return false;
    }
    return true;
}