#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <set>
#include <limits>
#include <cstdlib> // for system()
#include "routing.h"

using namespace std;

// ------------------------------------------------------------------
// Helper: clear console screen (cross‑platform)
// ------------------------------------------------------------------
void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// ------------------------------------------------------------------
// Helper: show graph statistics
// ------------------------------------------------------------------
void showStats(const Routing &r)
{
    if (r.getNodes().empty())
        return;
    cout << "Graph loaded: " << r.getNodes().size() << " nodes, ";
    int edgeCount = 0;
    for (const auto &[_, neighbors] : r.getGraph())
    {
        edgeCount += static_cast<int>(neighbors.size());
    }
    cout << edgeCount / 2 << " undirected edges\n";
    if (r.hasNegativeEdge())
    {
        cout << "⚠️  Graph contains negative edge weights.\n";
    }
}

// ------------------------------------------------------------------
// Helper: pause before returning to menu
// ------------------------------------------------------------------
void pressEnterToContinue()
{
    cout << "\nPress Enter to continue...";
    string dummy;
    getline(cin, dummy);
}

// ------------------------------------------------------------------
// Helper: print the whole graph
// ------------------------------------------------------------------
void printGraph(const Routing &r)
{
    for (const auto &[node, neighbors] : r.getGraph())
    {
        cout << "Edges from node " << node << ":\n";
        for (const auto &e : neighbors)
        {
            cout << "  -> " << e.v
                 << " (cost = " << e.total_cost() << ")\n";
        }
    }
    cout << endl;
}

// ------------------------------------------------------------------
// Helper: print distance map and parent pointers
// ------------------------------------------------------------------
void printDist(const map<int, double> &dist, int from,
               const map<int, int> &parent, int target = -1)
{
    if (dist.empty())
    {
        cout << "No distances available (negative cycle or unreachable).\n";
        return;
    }
    if (target != -1)
    {
        auto it = dist.find(target);
        if (it != dist.end() && it->second < numeric_limits<double>::infinity())
            cout << "Distance from " << from << " to " << target << " = " << it->second << "\n";
        else
            cout << "No path from " << from << " to " << target << ".\n";
    }
    else
    {
        cout << "Distances from node " << from << ":\n";
        for (const auto &[n, d] : dist)
        {
            cout << "  " << n << " : ";
            if (d == numeric_limits<double>::infinity())
                cout << "∞\n";
            else
                cout << d << "\n";
        }
    }

    cout << "\nParent pointers (for path reconstruction):\n";
    for (const auto &[v, u] : parent)
    {
        if (u != -1)
        {
            cout << "  parent of " << v << " is " << u << "\n";
        }
    }
    cout << endl;
}

// ------------------------------------------------------------------
// Helper: print a reconstructed path
// ------------------------------------------------------------------
void printPath(const vector<int> &path)
{
    if (path.empty())
    {
        cout << "No path.\n";
        return;
    }
    cout << "Path: ";
    for (size_t i = 0; i < path.size(); ++i)
    {
        if (i)
            cout << " -> ";
        cout << path[i];
    }
    cout << endl;
}

// ------------------------------------------------------------------
// File loading with negative‑cycle check
// ------------------------------------------------------------------
void getFiles(Routing &routing)
{
    // ----- Nodes -----
    while (true)
    {
        cout << "Enter nodes CSV file path (or q to quit): ";
        string path;
        getline(cin, path);
        if (path == "q")
            return; // early exit
        if (routing.getNodes(path))
        {
            if (!routing.getNodes().empty())
                break;
            cout << "Nodes file is empty. Please provide a valid file.\n";
        }
        else
        {
            cout << "Could not open or parse file: " << path << "\n";
        }
    }

    // ----- Edges -----
    while (true)
    {
        cout << "Enter edges CSV file path (or q to quit): ";
        string path;
        getline(cin, path);
        if (path == "q")
            return;

        if (!routing.getEdges(path))
        {
            cout << "Could not open or parse edges file: " << path << "\n";
            continue;
        }

        // Initialise and check for negative cycles
        routing.initialize();
        if (routing.hasNegativeCycle())
        {
            cout << "The graph contains negative cycles! "
                 << "Shortest paths are undefined. Please choose another file.\n";
            continue;
        }
        break; // success
    }
}

// ------------------------------------------------------------------
// Interactive node selection
// ------------------------------------------------------------------
int getNode(const Routing &routing)
{
    cout << "Available nodes: ";
    for (int n : routing.getNodes())
        cout << n << " ";
    cout << "\nEnter a node ID: ";
    string input;
    int node;
    while (getline(cin, input))
    {
        try
        {
            node = stoi(input);
            if (routing.getNodes().count(node))
                return node;
            cout << "Node " << node << " not in graph. Try again: ";
        }
        catch (...)
        {
            cout << "Invalid number. Try again: ";
        }
    }
    return -1; // should never happen in normal interactive use
}

// ------------------------------------------------------------------
// Collect multiple destinations (excluding source)
// ------------------------------------------------------------------
vector<int> getDestinations(const Routing &routing, int source)
{
    set<int> chosen;
    cout << "Select destination nodes (enter -1 to stop).\n";
    cout << "Available nodes: ";
    for (int n : routing.getNodes())
        cout << n << " ";
    cout << "\nAlready chosen: none\n";

    while (chosen.size() < routing.getNodes().size() - 1)
    {
        cout << "Next destination: ";
        string input;
        getline(cin, input);
        int node;
        try
        {
            node = stoi(input);
            if (node == -1)
                break;
            if (node == source)
            {
                cout << "Source node cannot be a destination. Try again.\n";
            }
            else if (routing.getNodes().count(node) == 0)
            {
                cout << "Node not in graph.\n";
            }
            else if (chosen.count(node))
            {
                cout << "Already selected.\n";
            }
            else
            {
                chosen.insert(node);
                cout << "Currently chosen: ";
                for (int n : chosen)
                    cout << n << " ";
                cout << "\n";
            }
        }
        catch (...)
        {
            cout << "Invalid input. Enter a number.\n";
        }
    }
    return vector<int>(chosen.begin(), chosen.end());
}

// ------------------------------------------------------------------
// Menu selection
// ------------------------------------------------------------------
int getOption()
{
    cout << "\n========================================\n"
         << " 1. Shortest path to one target node\n"
         << " 2. Shortest paths to all nodes\n"
         << " 3. Shortest cycle visiting chosen nodes (Nearest Neighbour)\n"
         << " 4. Show graph\n"
         << " 5. Change input files\n"
         << " 6. Exit\n"
         << "Enter your choice (1-6): ";

    string input;
    int opt;
    while (getline(cin, input))
    {
        try
        {
            opt = stoi(input);
            if (opt >= 1 && opt <= 6)
                return opt;
            cout << "Please enter a number between 1 and 6: ";
        }
        catch (...)
        {
            cout << "Invalid input. Enter a number: ";
        }
    }
    return 6;
}

// ------------------------------------------------------------------
// Main program
// ------------------------------------------------------------------
int main()
{
    Routing routing;
    clearScreen();
    cout << "===== Routing Engine =====\n\n";

    getFiles(routing);
    if (routing.getNodes().empty())
    {
        cout << "No graph loaded. Exiting.\n";
        return 0;
    }
    routing.initialize(); // ensure full initialisation
    showStats(routing);
    pressEnterToContinue();

    while (true)
    {
        clearScreen();
        showStats(routing);
        int option = getOption();
        cout << endl;

        // Operations that need a valid, cycle‑free graph
        if (option >= 1 && option <= 3)
        {
            if (routing.getNodes().empty())
            {
                cout << "No graph loaded. Use option 5 first.\n";
                pressEnterToContinue();
                continue;
            }
            if (routing.hasNegativeCycle())
            {
                cout << "Negative cycle detected – shortest paths undefined.\n";
                pressEnterToContinue();
                continue;
            }
        }

        switch (option)
        {
        case 1:
        {
            int source = getNode(routing);
            cout << "\nSelect target node:\n";
            int target = getNode(routing);

            map<int, double> dist;
            if (routing.hasNegativeEdge())
                dist = routing.bellmanFord(source);
            else
                dist = routing.dijkstra(source);

            printDist(dist, source, routing.getParent(), target);
            // Show path reconstruction
            vector<int> path = routing.reconstructPath(target);
            printPath(path);

            pressEnterToContinue();
            break;
        }
        case 2:
        {
            int source = getNode(routing);
            map<int, double> dist;
            if (routing.hasNegativeEdge())
                dist = routing.bellmanFord(source);
            else
                dist = routing.dijkstra(source);
            printDist(dist, source, routing.getParent());
            pressEnterToContinue();
            break;
        }
        case 3:
        {
            int start = getNode(routing);
            vector<int> dests = getDestinations(routing, start);
            if (dests.empty())
            {
                cout << "No destinations selected.\n";
                pressEnterToContinue();
                break;
            }
            auto [route, cost] = routing.nearestNeighbor(start, dests);
            cout << "Total cost: " << cost << "\nVisited order: ";
            for (int n : route)
                cout << n << " ";
            cout << endl;
            pressEnterToContinue();
            break;
        }
        case 4:
        {
            printGraph(routing);
            pressEnterToContinue();
            break;
        }
        case 5:
        {
            getFiles(routing);
            routing.initialize();
            break;
        }
        case 6:
        {
            cout << "Goodbye!\n";
            return 0;
        }
        default:
            break;
        }
    }
    return 0;
}