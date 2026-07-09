#include <iostream>
#include <vector>
#include "routing.h"
#include "graph_types.h"

using namespace std;
Routing routing;

void getFiles()
{

    while (routing.nodes.empty())
    {
        cout << "please provid the nodes csv file path : " << endl;
        string filepath;
        getline(cin, filepath);
        cout << endl;
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
        cout << endl;

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
}

void printGraph()
{
    for (auto &&[node, ls] : routing.graph)
    {
        cout << "Nodes from " << node << endl;
        for (auto &&e : ls)
        {
            cout << "from: " << e.u << " to: " << e.v << " distance: " << e.d << " trafic: " << e.t << " weather: " << e.w << endl;
        }
    }
    cout << endl;
}

void printDist(map<int, double> dist, int from, int node = -1)
{
    if (node != -1)
    {
        cout << "Distance from " << from << " to node " << node << " is: " << dist[node] << endl;
    }
    else
    {
        cout << "Distances from node " << from << endl;
        for (auto &&[n, dis] : dist)
        {
            cout << n << ": " << dis << endl;
        }
    }

    cout << endl
         << "Route is: " << endl;
    for (auto &&[v, u] : routing.parent)
    {
        if (v == -1 || u == -1)
        {
            continue;
        }

        cout << "parent of node " << v << " is " << u << endl;
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
    while (nodes.size() < routing.nodes.size() - 1)
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
    int node = 0;
    while (true)
    {
        getline(cin, input);

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
    cout << "5. change files" << endl;
    cout << "6. exit" << endl;
    cout << "enter a number between 1 and 5" << endl;
    string input;
    int option = 0;
    while (true)
    {
        getline(cin, input);
        try
        {

            option = stoi(input);
            if (option < 1 || option > 6)
            {
                cout << "please enter a number between 1 and 6!" << endl;
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

    getFiles();
    routing.initialize();
    printGraph();
    cout << endl;

    while (true)
    {
        int op = getOption();
        cout << endl;

        switch (op)
        {
        case 1:
        {
            int node = getNode();
            cout << endl;

            map<int, double> dist;
            cout << "Select the node to go" << endl;
            int to_node = getNode();
            cout << endl;

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
            cout << endl;

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
            cout << "Total cost is " << res.second << endl
                 << endl;
            cout << "Route : " << endl;

            for (auto &&r : res.first)
            {
                cout << r << " ";
            }
            cout << endl
                 << endl;
            break;
        }
        case 4:
        {
            printGraph();
            break;
        }
        case 5:
        {
            getFiles();
            break;
        }
        case 6:
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
