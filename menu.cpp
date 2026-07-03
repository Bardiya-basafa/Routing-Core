#include <iostream>
#include <vector>
#include "routing.h"
#include "graph_types.h"

using namespace std;
Routing routing;

void printGraph()
{
    for (int i = 0; i < routing.edges.size(); i++)
    {
        if (i % 2 == 1)
        {
            continue;
        }
        edge e = routing.edges[i];

        cout << "from: " << e.u << " to: " << e.v << " distance: " << e.d << " trafic: " << e.t << " weather: " << e.w << endl;
    }
}

void printDist(map<int, double> dist, int from, int node = -1)
{
    if (node)
    {
        cout << "Distance from " << from << " to node " << node << "is: " << dist[node] << endl;
    }
    else
    {
        cout << "Distances from node " << from << endl;
        for (auto &&[n, dis] : dist)
        {
            cout << n << ": " << dis << endl;
        }
    }
    cout << "Route is: " << endl;
    for (auto &&[v, u] : routing.parent)
    {
        cout << "parent of node " << u << " is " << v << endl;
    }
    cout << endl;
}

vector<int> getDestinations(int from)
{
    set<int> nodes;
    cout << "Select nodes from all the nodes for destinations." << endl;
    for (auto &&n : routing.nodes)
    {
        cout << n << " ";
    }
    cout << endl;

    cout << "For stop count enter -1" << endl;
    string input;
    int node;
    while (true)
    {
        getline(cin, input);
        try
        {
            node = stoi(input);
            if (node == -1)
            {
                break;
            }
            else if (nodes.find(node) != nodes.end() || node == from || routing.nodes.find(node) == routing.nodes.end())
            {
                cout << "Please enter a new and valid node" << endl;
            }
            else
            {
                nodes.insert(node);
            }
        }
        catch (const std::exception &e)
        {
            cout << "Please enter a valid number" << endl;
        }
    }
    vector<int> dest(nodes.begin(), nodes.end());
    return dest;
}

int getNode()
{
    cout << "Choose a node: " << endl;
    for (auto &&node : routing.nodes)
    {
        cout << node << " ";
    }
    cout << endl;
    string input;
    getline(cin, input);
    int node = 0;
    while (true)
    {
        try
        {
            node = stoi(input);
            if (routing.nodes.find(node) == routing.nodes.end())
            {
                cout << "Please enter a valid node" << endl;
            }
            else
            {
                break;
            }
        }
        catch (const std::exception &e)
        {
            cout << "Please enter a valid number" << endl;
        }
    }
    return node;
}

int getOption()
{
    cout << "Choose an option: " << endl;
    cout << "1. shortest path to one node from a specific node" << endl;
    cout << "2. shortest path to all nodes from a specific node" << endl;
    cout << "3. shortest cycle from one node to various destinations" << endl;
    cout << "4. see the graph" << endl;
    cout << "5. exit" << endl;
    cout << "enter a number between 1 and 5" << endl;
    string input;
    getline(cin, input);
    int option = 0;
    while (true)
    {
        try
        {

            option = stoi(input);
            if (option < 1 || option > 5)
            {
                cout << "please enter a number between 1 and 5!" << endl;
            }
            else
            {
                break;
            }
        }
        catch (const std::exception &e)
        {
            cout << "please enter a number!" << endl;
        }
    }
    return option;
}

void menu()
{
    cout << "Welcome to the routing core!" << endl;

    while (routing.nodes.empty())
    {
        cout << "please provid the nodes csv file path : " << endl;
        string filepath;
        getline(cin, filepath);
        auto res = routing.getNodes(filepath);
        if (!res)
        {
            cout << "File cant be opend or empty path : " << filepath << endl;
        }
        else if (routing.nodes.empty())
        {
            cout << "Nodes are empty please fill the file for routing" << endl;
        }
    }

    while (true)
    {

        cout << "please provid the edges csv file path : " << endl;
        string filepath;
        getline(cin, filepath);
        auto res = routing.getEdges(filepath);
        if (!res)
        {
            cout << "File cant be opend or empty, path is: " << filepath << endl;
        }
        else if (routing.negetive_cycle)
        {
            cout << "Graph has negetive cycles and cant find any shortest path provide another file" << endl;
        }
        else
        {
            break;
        }
    }

    routing.initialize();
    printGraph();
    cout << endl;

    while (true)
    {
        int op = getOption();
        switch (op)
        {
        case 1:
        {
            int node = getNode();
            map<int, double> dist;
            cout << "Select the node to go" << endl;
            int to_node = getNode();

            if (routing.negetive_edge)
            {
                dist = routing.bellmanFord(node);
            }
            else
            {
                dist = routing.dijkstra(node);
            }
            printDist(dist, node, to_node);
            break;
        }

        case 2:
        {
            int node = getNode();
            map<int, double> dist;

            if (routing.negetive_edge)
            {
                dist = routing.bellmanFord(node);
            }
            else
            {
                dist = routing.dijkstra(node);
            }
            printDist(dist, node);
            break;
        }
        case 3:
        {
            int node = getNode();
            vector<int> dests = getDestinations(node);
            auto res = routing.nearestNeighbor(node, dests);
            cout << "Total cost is " << res.second << endl;
            cout << "Route : " << endl;
            for (auto &&r : res.first)
            {
                cout << r << " ";
            }

            break;
        }
        case 4:
        {
            printGraph();
            break;
        }
        case 5:
        {
            cout << "Good bye!";
            return;
            break;
        }
        default:
            break;
        }
    }
}
