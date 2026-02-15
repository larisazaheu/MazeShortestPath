#include "Graph.h"
#include <fstream>

bool Graph::isValid(int i, int j) {
    return (i >= 0 && j >= 0 && i < (int)matrix.size() && j < (int)matrix[0].size());
}

void Graph::readFromString(const std::string& data) {
    matrix.clear();
    std::istringstream fin(data);
    std::string line;

    while (std::getline(fin, line)) {
        std::vector<int> row;
        for (char c : line) {
            if (c >= '0' && c <= '3')
                row.push_back(c - '0');
        }
        if (!row.empty())
            matrix.push_back(row);
    }

    if (matrix.empty()) {
        qDebug() << "Invalid maze!";
    }
}


void Graph::buildGraph() {
    int n = matrix.size();
    int m = matrix[0].size();

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (matrix[i][j] == 0) continue;
            Node node(i, j);
            if (matrix[i][j] == 3) start = node;
            if (matrix[i][j] == 2) exits.push_back(node);

            int dx[4] = { -1, 1, 0, 0 };
            int dy[4] = { 0, 0, -1, 1 };
            for (int d = 0; d < 4; d++) {
                int ni = i + dx[d], nj = j + dy[d];
                if (isValid(ni, nj) && matrix[ni][nj] != 0) {
                    adj[node].push_back(Node(ni, nj));
                }
            }
        }
    }
}

void Graph::bfs() {
    std::queue<Node> V;
    std::set<Node> W;

    parent.clear();
    level.clear();

    for (auto const& [node, adj_list] : adj) {
        level[node] = std::numeric_limits<int>::max();
    }

    V.push(start);
    parent[start] = Node(-1, -1);
    level[start] = 0;

    while (!V.empty()) {
        Node x = V.front();
        V.pop();

        W.insert(x);
        for (auto y : adj[x]) {

            if (W.find(y) == W.end() && parent.find(y) == parent.end()) {
                V.push(y);
                parent[y] = x;
                level[y] = level[x] + 1;
            }
        }
    }
}

std::vector<Node> Graph::getPathTo(Node exit) {
    std::vector<Node> path;
    Node current = exit;
    while (current.x != -1 && current.y != -1) {
        path.push_back(current);
        current = parent[current];
    }
    std::reverse(path.begin(), path.end());
    return path;
}
