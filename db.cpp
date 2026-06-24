#include <sqlite3.h>
#include <iostream>
#include <filesystem>

using namespace std;

int main()
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
        return 1;
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
    return 0;
}