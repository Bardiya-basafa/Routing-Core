# Routing Core

A command-line **graph routing engine** written in modern C++ (C++17). It loads a road-network-style graph from CSV files and answers routing questions on it:

- the shortest path between two locations,
- shortest distances from one location to every other location,
- a short **round trip** that visits a chosen set of locations and returns to the start (a Travelling-Salesman-style problem solved with the Nearest Neighbour heuristic).

The engine picks the shortest-path algorithm on its own. It uses **Dijkstra** when all weights are non-negative and switches to **Bellman–Ford** when negative weights appear. It also detects **negative cycles**, which make shortest paths undefined, and refuses to route on such graphs.

---

## Table of Contents

- [Features](#features)
- [Project Structure](#project-structure)
- [Graph Model](#graph-model)
- [Input File Formats](#input-file-formats)
- [Algorithms](#algorithms)
- [Architecture](#architecture)
- [Build and Run](#build-and-run)
- [Usage Walkthrough](#usage-walkthrough)
- [Test Graphs](#test-graphs)
- [Error Handling](#error-handling)
- [Complexity Summary](#complexity-summary)
- [Known Limitations and Possible Improvements](#known-limitations-and-possible-improvements)

---

## Features

| Feature | Description |
|---|---|
| CSV graph loading | Nodes and edges are read from two CSV files, and both headers are checked. |
| Composite edge cost | Each edge cost combines **distance × traffic × weather**, plus the **delay** of the node you arrive at. |
| Automatic algorithm choice | Dijkstra for non-negative graphs, Bellman–Ford when any effective edge weight is negative. |
| Negative-cycle detection | Runs once when the graph loads. Graphs with negative cycles are rejected. |
| Path reconstruction | Parent pointers rebuild the actual node sequence, not just the cost. |
| Multi-destination tour | Nearest Neighbour heuristic builds a closed tour through selected nodes. |
| Interactive TUI menu | Validated input, screen clearing, and graph statistics (node and edge counts). |
| Hot reload of input files | Load a different graph at runtime without restarting. |

---

## Project Structure

```
Routing-Core/
├── docs/
│   ├── doc.odt          # Project report (OpenDocument)
│   └── doc.pdf          # Project report (PDF)
└── src/
    ├── graph_types.h    # `edge` struct and its cost function
    ├── file.h / file.cpp        # CSV parsing for nodes and edges
    ├── routing.h / routing.cpp  # Routing engine (Dijkstra, Bellman–Ford, NN-TSP)
    ├── main.cpp         # Interactive console UI / menu
    ├── run.sh           # Build-and-run helper script
    └── tests/           # Five sample graphs (N_nodes.csv + N_edges.csv)
```

---

## Graph Model

The network is an **undirected weighted graph**. Each undirected edge in the CSV is stored internally as **two directed edges** (`u → v` and `v → u`) in an adjacency list:

```cpp
std::map<int, std::vector<edge>> graph_;   // node -> outgoing edges
std::unordered_map<int, double>  delays_;  // node -> delay
std::set<int>                    nodes_;   // all node IDs
```

### Edge structure (`graph_types.h`)

```cpp
struct edge {
    int u;     // source node
    int v;     // target node
    double d;  // distance multiplier
    double t;  // traffic multiplier
    double w;  // weather multiplier

    double total_cost() const { return d * t * w; }
};
```

### Effective weight

The weight the algorithms use when they relax an edge `u → v` is:

```
weight(u → v) = distance × traffic × weather + delay(v)
```

The node delay models a cost paid at a location, such as waiting time at an intersection. It can be **negative**, for example a bonus or a shortcut, so effective edge weights can also be negative. That is why the engine supports Bellman–Ford.

Because the delay belongs to the **destination** node, the two directions of one undirected edge can have different effective weights:

```
weight(u → v) = cost + delay(v)
weight(v → u) = cost + delay(u)
```

---

## Input File Formats

The parser needs two CSV files, each with an **exact header line**. Windows `\r` line endings are stripped automatically.

### Nodes file

```csv
node,delay
0,0
1,-3
2,0
```

| Column | Type | Meaning |
|---|---|---|
| `node` | int | Unique node ID |
| `delay` | double | Cost added when arriving at this node (may be negative) |

### Edges file

```csv
source,target,distance,traffic,weather
0,1,2,2,1
0,2,4,1,1
```

| Column | Type | Meaning |
|---|---|---|
| `source` | int | One endpoint |
| `target` | int | Other endpoint |
| `distance` | double | Base length of the road |
| `traffic` | double | Traffic multiplier |
| `weather` | double | Weather multiplier |

> Load the nodes file **first**. The edge parser looks up each endpoint's delay to decide whether any effective weight is negative.

---

## Algorithms

### 1. Dijkstra's algorithm (`Routing::dijkstra`)

Used when **no** effective edge weight is negative.

- All distances start at `+∞` and all parents at `-1`. The source distance is `0`.
- A **min-heap** (`std::priority_queue` with `std::greater`) of `(distance, node)` pairs selects the closest unsettled node.
- **Lazy deletion:** a popped entry whose distance is already larger than the best known distance is skipped (`if (d > dist[u]) continue;`).
- Each relaxation updates `dist[v]` and `parent[v]`.

**Complexity:** `O((V + E) log V)`

### 2. Bellman–Ford algorithm (`Routing::bellmanFord`)

Used when at least one effective edge weight is negative.

- Relaxes every edge up to `V − 1` times.
- **Early exit:** stops as soon as a full pass makes no update.
- **Negative-cycle check:** one extra pass is run. If any edge can still be relaxed, a negative cycle is reachable. The function then clears the parent map and returns an **empty map** to signal this.

**Complexity:** `O(V · E)`

### 3. Negative-cycle detection on load (`Routing::initialize`)

After both files load, `initialize()` runs Bellman–Ford once from the first node and sets `has_negative_cycle_`. The UI refuses graphs with negative cycles and asks for a different edges file.

### 4. Choosing the algorithm (`Routing::shortestPathCost`)

```cpp
dist = has_negative_edge_ ? bellmanFord(s) : dijkstra(s);
```

The `has_negative_edge_` flag is computed during CSV parsing. An edge is negative if `cost + delay(target)` is below zero in either direction.

### 5. Path reconstruction (`Routing::reconstructPath`)

Every shortest-path run records `last_source_` and fills `parent_`. To rebuild the path to `dest`, the code follows parent pointers back to `-1` and then reverses the list:

- `dest == source` returns `[source]`
- `parent[dest] == -1` means the node is unreachable and returns an empty path
- otherwise it returns `source → … → dest`

### 6. Multi-destination tour: Nearest Neighbour heuristic (`Routing::nearestNeighbor`)

This is a greedy approximation of the **Travelling Salesman Problem** on the chosen destinations:

```
route ← [start]; current ← start; unvisited ← destinations
while unvisited is not empty:
    next ← argmin over n in unvisited of shortestPathCost(current, n)
    route.append(next); total += cost; current ← next; unvisited.remove(next)
total += shortestPathCost(current, start)      // close the tour
route.append(start)
```

The cost between two stops is the **true shortest-path cost** through the graph (Dijkstra or Bellman–Ford), not the cost of a direct edge. Intermediate nodes may therefore be passed through.

- **Complexity:** `O(k² · SP)`, where `k` is the number of destinations and `SP` is the cost of one shortest-path run.
- **Quality:** a heuristic, so the result is not guaranteed to be optimal, but it is fast and usually reasonable.

---

## Architecture

```
┌───────────────────────── main.cpp (UI layer) ─────────────────────────┐
│ getFiles()  getOption()  getNode()  getDestinations()                 │
│ printGraph() printDist() printPath() showStats()                      │
└───────────────────────────────┬───────────────────────────────────────┘
                                │ uses
┌───────────────────────────────▼───────────────────────────────────────┐
│ Routing (routing.h / routing.cpp)                                     │
│  getNodes/getEdges → initialize → dijkstra / bellmanFord              │
│  shortestPathCost   reconstructPath   nearestNeighbor                 │
│  read-only accessors for the UI (getGraph, getParent, flags...)       │
└───────────────────────────────┬───────────────────────────────────────┘
                                │ delegates parsing to
┌───────────────────────────────▼───────────────────────────────────────┐
│ file.h / file.cpp: readNodes(), readEdges()                           │
│ header validation, type conversion, negative-edge detection           │
└───────────────────────────────┬───────────────────────────────────────┘
                                │ produces
                       graph_types.h: struct edge
```

- **Separation of concerns:** parsing, algorithms and presentation live in separate translation units.
- **Encapsulation:** the `Routing` class owns all graph state. The UI only reads it through `const` accessors.

---

## Build and Run

### Requirements

- A C++17 compiler (`g++` 8+ or `clang++` 7+). The code uses structured bindings (`auto [d, u] = ...`).
- Linux, macOS or Windows. The screen-clear helper handles both `cls` and `clear`.

### Using the script

```bash
cd src
sh ./run.sh
```

`run.sh` compiles and launches the program:

```bash
g++ ./main.cpp ./routing.cpp ./file.cpp -o RoutingCore && clear && ./RoutingCore
```

### Manual build

```bash
cd src
g++ -std=c++17 -O2 main.cpp routing.cpp file.cpp -o RoutingCore
./RoutingCore
```

File paths are resolved relative to the **directory you run the program from**. When you run it from `src/`, use paths like `tests/1_nodes.csv`.

---

## Usage Walkthrough

```
===== Routing Engine =====

Enter nodes CSV file path (or q to quit): tests/1_nodes.csv
Enter edges CSV file path (or q to quit): tests/1_edges.csv
Graph loaded: 20 nodes, 33 undirected edges

========================================
 1. Shortest path to one target node
 2. Shortest paths to all nodes
 3. Shortest cycle visiting chosen nodes (Nearest Neighbour)
 4. Show graph
 5. Change input files
 6. Exit
Enter your choice (1-6):
```

| Option | What it does |
|---|---|
| **1** | Asks for a source and a target. Prints the distance, all parent pointers and the reconstructed path, for example `Path: 0 -> 3 -> 7`. |
| **2** | Asks for a source. Prints the distance to every node (`INF` if unreachable) and the parent pointers. |
| **3** | Asks for a start node, then destinations one at a time (`-1` to finish). Prints the visiting order and the total cost of the closed tour. |
| **4** | Prints every adjacency list with each edge's raw cost. |
| **5** | Loads a different pair of CSV files and runs the negative-cycle check again. |
| **6** | Exits. |

Every numeric input is validated. Bad numbers, unknown node IDs, choosing the source as a destination, and duplicate destinations each produce a message and a new prompt.

---

## Test Graphs

`src/tests/` has five sample graphs that cover different cases:

| Test | Nodes | Edges | Scenario |
|---|---|---|---|
| `1_*` | 20 | 33 | General connected graph with non-negative weights (exercises **Dijkstra**). |
| `2_*` | 12 | 13 | Negative node delays (`-6`, `-3`, `-15`) produce strongly negative weights and a **negative cycle**, so the graph is rejected. |
| `3_*` | 15 | 16 | Two rings with negative delays on nodes 1, 3 and 7. Edges such as `0–1` (`1 → 0 = 1`, `0 → 1 = 1 − 3 = −2`) form negative 2-cycles, so this graph is also **rejected** by the negative-cycle check. |
| `4_*` | 6 | 4 | **Disconnected** graph: unreachable nodes report `INF` or "No path". |
| `5_*` | 15 | 14 | A simple **path/chain** graph (0–1–2–…–14), handy for checking path reconstruction. |

---

## Error Handling

- **Missing or unreadable file:** the UI prints a message and asks again.
- **Empty file or wrong header:** a precise error is printed to `stderr`, such as `Expected: node,delay`.
- **Malformed numbers:** `std::stoi` and `std::stod` exceptions are caught, partial data is cleared, and loading fails cleanly.
- **Negative cycle:** detected on load. The graph is rejected and shortest-path operations are blocked.
- **Unreachable targets:** shown as `INF` or `No path`.

---

## Complexity Summary

| Operation | Time | Space |
|---|---|---|
| Load graph | `O(V + E)` | `O(V + E)` |
| Dijkstra | `O((V + E) log V)` | `O(V)` |
| Bellman–Ford | `O(V · E)` | `O(V)` |
| Negative-cycle check | `O(V · E)` | `O(V)` |
| Path reconstruction | `O(V)` | `O(V)` |
| Nearest Neighbour tour (k stops) | `O(k² · SP)` | `O(V + k)` |

---

## Known Limitations and Possible Improvements

- **Undirected graphs and negative weights:** an undirected edge whose two directions add up to a negative value (`w(u→v) + w(v→u) < 0`) is a negative 2-cycle. Graphs with negative weights therefore only work when node delays are small compared with edge costs.
- **Detection from one source:** `initialize()` runs Bellman–Ford from the first node only, so a negative cycle in a separate connected component is caught later, when a query runs from that component.
- **Repeated work in the tour:** `nearestNeighbor` calls `shortestPathCost` again for every candidate. Caching one full shortest-path run per stop would turn `k²` runs into `k`.
- **Tour quality:** the heuristic could be improved with **2-opt** or **Or-opt** local search, or replaced by Held–Karp dynamic programming for small `k`.
- The tour shows only the order of stops. The full node-by-node route between stops could also be reconstructed.

---

## Documentation

A detailed project report is available in [`docs/doc.pdf`](docs/doc.pdf). The editable source is [`docs/doc.odt`](docs/doc.odt).
