#include <iostream>
#include <string>
#include <vector>
// #include <sqlite3.h>
#include <filesystem>
#include <queue>

using namespace std;
using pii = pair<int, int>;

struct edge
{
    int u;
    int v;
    double d;
    double t;
    double w;
    double total_cost()
    {
        // return d * t * w;
        return d;
    }
};

vector<double> dijstra(vector<vector<edge>> &graph, int s, vector<int> &parent, vector<double> &delays);

vector<double> bellmanFord(vector<edge> &graph, int s, vector<int> &parent, vector<double> &delays, int V);

// void getFromDb(vector<int> &nodes, vector<double> &delays, vector<edge> &edges)

int main(int argc, char const *argv[])
{

    vector<int> nodes = {0, 1, 2, 3, 4};
    vector<double> delays = {0, 0, 0, 0, 0};
    vector<edge> edges = {
        {0, 1, 4, 0, 0},
        {0, 2, 8, 0, 0},
        {1, 2, 3, 0, 0},
        {1, 4, 6, 0, 0},
        {4, 3, 10, 0, 0},
        {2, 3, 2, 0, 0},
    };

    // complete graph
    vector<vector<edge>> graph(nodes.size());

    for (auto &&u : nodes)
    {
        for (auto &&edg : edges)
        {
            if (edg.u == u)
            {
                graph[u].push_back(edg);
            }
            else if (edg.v == u)
            {
                edge new_edg = edg;
                swap(new_edg.u, new_edg.v);
                graph[u].push_back(new_edg);
            }
        }
    }

    // dijstra
    // vector<int> parent(graph.size());
    // auto dist = dijstra(graph, 0, parent, delays);

    // for (auto &&i : parent)
    // {
    //     cout << i << " ";
    // }

    // bellman ford
    vector<int> parent(nodes.size());

    auto dist1 = bellmanFord(edges, 0, parent, delays, nodes.size());

    for (auto &&i : parent)
    {
        cout << i << " ";
    }

    // getFromDb(nodes, delays, edges);

    return 0;
}

vector<double> dijstra(vector<vector<edge>> &graph, int s, vector<int> &parent, vector<double> &delays)
{
    vector<double> dist(graph.size(), __INT_MAX__);
    dist[s] = 0;
    priority_queue<pii, vector<pii>, greater<pii>> pq;
    pq.emplace(make_pair(0, s));
    while (!pq.empty())
    {
        pii p = pq.top();
        pq.pop();
        int u = p.second;
        int d = p.first;
        if (d > dist[u])
        {
            continue;
        }

        for (auto &&e : graph[u])
        {
            int v = e.v;
            int w = e.total_cost() + delays[v];

            if (dist[u] + w < dist[v])
            {
                parent[v] = u;
                dist[v] = dist[u] + w;
                pq.emplace(make_pair(dist[v], v));
            }
        }
    }

    return dist;
}

vector<double> bellmanFord(vector<edge> &graph, int s, vector<int> &parent, vector<double> &delays, int V)
{

    vector<double> dist(V, __INT_MAX__);
    dist[s] = 0;

    for (int i = 0; i < V; i++)
    {
        for (auto &&e : graph)
        {
            int u = e.u;
            int v = e.v;
            int w = e.total_cost();
            if (dist[u] + w < dist[v])
            {

                if (i == V - 1)
                {
                    return {-1};
                    parent.clear();
                }
                dist[v] = dist[u] + w;
                parent[v] = u;
            }
        }
    }

    return dist;
}

// void getFromDb(vector<int> &nodes, vector<double> &delays, vector<edge> &edges)
// {

//     const string dbPath = "graph.db";

//     cout << "Database location: "
//          << filesystem::absolute(dbPath) << endl;

//     sqlite3 *db = nullptr;
//     int rc = sqlite3_open(dbPath.c_str(), &db);

//     if (rc != SQLITE_OK)
//     {
//         cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
//         return;
//     }

//     cout << "Database opened successfully!" << endl;

//     sqlite3_stmt *stmt_nodes;

//     string sql_nodes = "SELECT id, delay FROM nodes";

//     sqlite3_prepare_v2(db, sql_nodes.c_str(), -1, &stmt_nodes, NULL);

//     while (sqlite3_step(stmt_nodes) == SQLITE_ROW)
//     {

//         int u = sqlite3_column_int(stmt_nodes, 0);
//         double delay = sqlite3_column_double(stmt_nodes, 1);

//         nodes.push_back(u - 1);
//         delays.push_back(delay);
//     }

//     sqlite3_finalize(stmt_nodes);

//     sqlite3_stmt *stmt_edges;

//     string sql_edges = "SELECT source, target, distance, trafic, weather FROM edges";
//     sqlite3_prepare_v2(db, sql_edges.c_str(), -1, &stmt_edges, NULL);

//     while (sqlite3_step(stmt_edges) == SQLITE_ROW)
//     {
//         int u = sqlite3_column_int(stmt_edges, 1);

//         int v = sqlite3_column_int(stmt_edges, 2);

//         double d = sqlite3_column_double(stmt_edges, 3);

//         double trafic = sqlite3_column_double(stmt_edges, 4);

//         double weather = sqlite3_column_double(stmt_edges, 5);

//         edge e = {u - 1, v - 1, d, trafic, weather};
//         edge erev = {v - 1, u - 1, d, trafic, weather};
//         edges.push_back(e);
//         edges.push_back(erev);
//     }

//     sqlite3_finalize(stmt_edges);

//     // cout << nodes.size() << endl;
//     // cout << edges.size() << endl;
// }
