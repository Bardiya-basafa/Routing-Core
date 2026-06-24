#include "database.h"
#include <sqlite3.h>
#include <filesystem>
#include <iostream>
#include <string>

using namespace std;

void getFromDb(vector<int> &nodes, vector<double> &delays, vector<edge> &edges)
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

        int u = sqlite3_column_int(stmt_nodes, 0);
        double delay = sqlite3_column_double(stmt_nodes, 1);

        nodes.push_back(u - 1);
        delays.push_back(delay);
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

        edge e = {u - 1, v - 1, d, trafic, weather};
        edge erev = {v - 1, u - 1, d, trafic, weather};
        edges.push_back(e);
        edges.push_back(erev);
    }

    sqlite3_finalize(stmt_edges);

    sqlite3_close(db);
}

void createDb()
{

    // Create database in current directory
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

    // Create a table
    const char *sql = R"(
        CREATE TABLE nodes (
            id INTEGER PRIMARY KEY,
            delay REAL
        );

        CREATE TABLE edges (
            id INTEGER PRIMARY KEY,
            source INTEGER,
            target INTEGER,
            distance REAL,
            trafic REAL,
            weather REAL,
            FOREIGN KEY(source) REFERENCES nodes(id),
            FOREIGN KEY(target) REFERENCES nodes(id)
        );
    )";

    char *errMsg = nullptr;
    rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK)
    {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    }
    else
    {
        cout << "Tables created successfully!" << endl;
    }

    sqlite3_close(db);
}