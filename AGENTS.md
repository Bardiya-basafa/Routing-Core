# Base44 Dev Environment

## Project
C++ routing engine (Dijkstra, Bellman-Ford, nearest-neighbor TSP) that reads graph CSVs.
Originally an interactive CLI (`src/main.cpp`); a web adapter exposes it on port 3000 for the preview.

## Files Added by Base44
- `src/adapter.cpp` — C++ adapter: loads graph files, runs algorithms, outputs JSON
- `web/app.py` — Flask server that calls the adapter binary
- `web/static/index.html` — single-page web UI
- `Dockerfile.base44` — image with g++ and Flask (no source baked in)
- `docker-compose.base44.yml` — compose setup

## Running
```
docker compose -f docker-compose.base44.yml up -d --build
```
The compose command compiles `src/adapter.cpp` + existing source into `src/RoutingAdapter`, then runs Flask on port 3000.

## Verifying
- `curl http://localhost:3000/` returns the web UI
- `curl -X POST http://localhost:3000/api/load -H 'Content-Type: application/json' -d '{"graph_id":1}'` returns graph JSON

## No External Secrets
No external services. No credentials required.

## Notes
- C++ source changes require container restart (recompiles adapter at startup)
- Web files (HTML/Python) hot-reload via Flask debug mode
- `routing.h` has a `getDelays()` getter added for the adapter (one line, no logic change)
- Test graph data is in `src/tests/` (1–5 nodes/edges CSV pairs)
