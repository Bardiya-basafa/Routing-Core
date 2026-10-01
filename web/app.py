import json
import os
import subprocess

from flask import Flask, request, jsonify

app = Flask(__name__, static_folder="static", static_url_path="")

ADAPTER = "/app/src/RoutingAdapter"
TESTS_DIR = "/app/src/tests"


def graph_files(graph_id):
    return (
        os.path.join(TESTS_DIR, f"{graph_id}_nodes.csv"),
        os.path.join(TESTS_DIR, f"{graph_id}_edges.csv"),
    )


def run_adapter(cmd, graph_id, *extra):
    nodes, edges = graph_files(graph_id)
    if not os.path.exists(nodes) or not os.path.exists(edges):
        return {"error": f"Graph {graph_id} not found"}
    try:
        result = subprocess.run(
            [ADAPTER, cmd, nodes, edges, *extra],
            capture_output=True,
            text=True,
            timeout=10,
        )
        if result.returncode != 0:
            return {"error": result.stderr.strip() or "Adapter error"}
        return json.loads(result.stdout.strip())
    except json.JSONDecodeError as e:
        return {"error": f"Invalid JSON from adapter: {e}"}
    except Exception as e:
        return {"error": str(e)}


@app.route("/")
def index():
    return app.send_static_file("index.html")


@app.route("/api/graphs")
def list_graphs():
    graphs = []
    for i in range(1, 6):
        nodes, edges = graph_files(i)
        if os.path.exists(nodes) and os.path.exists(edges):
            graphs.append({"id": i, "name": f"Test Graph {i}"})
    return jsonify(graphs)


@app.route("/api/load", methods=["POST"])
def load():
    data = request.json
    return jsonify(run_adapter("graph", data["graph_id"]))


@app.route("/api/path", methods=["POST"])
def path():
    data = request.json
    return jsonify(
        run_adapter("path", data["graph_id"], str(data["source"]), str(data["target"]))
    )


@app.route("/api/all", methods=["POST"])
def all_distances():
    data = request.json
    return jsonify(run_adapter("all", data["graph_id"], str(data["source"])))


@app.route("/api/tsp", methods=["POST"])
def tsp():
    data = request.json
    dests = ",".join(str(d) for d in data["destinations"])
    return jsonify(run_adapter("tsp", data["graph_id"], str(data["start"]), dests))


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=3000, debug=True)
