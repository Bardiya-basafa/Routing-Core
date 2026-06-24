#include <iostream>
#include <string>
#include <vector>
#include <sqlite3.h>
#include <filesystem>
#include <queue>

using namespace std;
using pii = pair<int, int>;

struct node
{
    int u;
    double delay;
};

struct edge
{
    node u;
    node v;
    double d;
    double t;
    double w;
    double total_cost()
    {
        return d * t * w + v.delay;
    }
};

void getFromDb(vector<node> &nodes, vector<edge> &edges)
{

    const string dbPath = "graph.db";

    cout << "Database location: "
         << filesystem::absolute(dbPath) << endl;

    sqlite3 *db = nullptr;
    int rc = sqlite3_open(dbPath.c_str(), &db);

    if (rc != SQLITE_OK)
    {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        return;
    }

    cout << "Database opened successfully!" << endl;

    sqlite3_stmt *stmt_nodes;

    string sql_nodes = "SELECT id, delay FROM nodes";

    sqlite3_prepare_v2(db, sql_nodes.c_str(), -1, &stmt_nodes, NULL);

    while (sqlite3_step(stmt_nodes) == SQLITE_ROW)
    {
        node n;

        n.u = sqlite3_column_int(stmt_nodes, 0);
        n.delay = sqlite3_column_double(stmt_nodes, 1);

        nodes.push_back(n);
    }

    sqlite3_finalize(stmt_nodes);

    sqlite3_stmt *stmt_edges;

    string sql_edges = "SELECT source, target, distance, trafic, weather FROM edges";
    sqlite3_prepare_v2(db, sql_edges.c_str(), -1, &stmt_edges, NULL);

    while (sqlite3_step(stmt_edges) == SQLITE_ROW)
    {
        int u = sqlite3_column_int(stmt_edges, 1);

        int v = sqlite3_column_int(stmt_edges, 2);

        double d = sqlite3_column_double(stmt_edges, 3);

        double trafic = sqlite3_column_double(stmt_edges, 4);

        double weather = sqlite3_column_double(stmt_edges, 5);

        edge e = {nodes[u - 1], nodes[v - 1], d, trafic, weather};
        edges.push_back(e);
    }

    // cout << nodes.size() << endl;
    // cout << edges.size() << endl;
}

void Dijstra(vector<edge> &graph, node source)
{
    vector<int> dist(graph.size(), __INT_MAX__);
    vector<int> parent(graph.size(), -1);
    int s = source.u - 1;
    dist[s] = 0;
    priority_queue<pii, vector<pii>, greater<pii>> pq;
    pq.push(make_pair(0, s));
    while (!pq.empty())
    {
        auto [du, u] = pq.top();
        if (du!=dist[u])
        {
            
        }
        
    }
}

int main(int argc, char const *argv[])
{

    vector<node> nodes;
    vector<edge> edges;
    getFromDb(nodes, edges);

    return 0;
}
