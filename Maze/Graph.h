#pragma once

#include <vector>
#include <QDebug>
#include <queue>
#include <map>
#include <utility>
#include <sstream>
#include <string>

struct Node {
    int x, y;
    Node(int _x = 0, int _y = 0) : x(_x), y(_y) {}
    bool operator<(const Node& other) const {
        if (x == other.x) return y < other.y;
        return x < other.x;
    }
};

class Graph {
public:
    std::map<Node, std::vector<Node>> adj;
    Node start;
    std::vector<Node> exits;
    std::map<Node, Node> parent;
    std::map<Node, int> level;
    std::vector<std::vector<int>> matrix;

    void readFromString(const std::string& data);
    void buildGraph();
    void bfs();
    std::vector<Node> getPathTo(Node exit);
    bool isValid(int i, int j);
};