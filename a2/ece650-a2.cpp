#include <iostream>
#include <vector>
#include <regex>
#include <string>
#include <list>
#include <climits>

using namespace std;

class Graph {
    int numVertices;
    vector<int> *edges;

public:
    Graph() : edges(NULL) {}

    void refreshGraph(int v);

    void addEdge(int x, int y);

    bool bfs(int src, int dest, int v, int pred[], int dist[]);

    void printShortestDistance(int s, int dest);

    void deleteEdge();

};

void Graph::refreshGraph(int v) {
    deleteEdge();
    numVertices = v;
    edges = new vector<int>[numVertices + 1];
}

void Graph::addEdge(int x, int y) {
    edges[x].push_back(y);
    edges[y].push_back(x);
}

bool Graph::bfs(int src, int dest, int v, int pred[], int dist[]) {
    list<int> queue;

    bool visited[v];

    for (int i = 0; i < v; i++) {
        visited[i] = false;
        dist[i] = INT_MAX;
        pred[i] = -1;
    }

    visited[src] = true;
    dist[src] = 0;
    queue.push_back(src);

    while (!queue.empty()) {
        int u = queue.front();
        queue.pop_front();
        for (unsigned i = 0; i < edges[u].size(); i++) {
            if (!visited[edges[u][i]]) {
                visited[edges[u][i]] = true;
                dist[edges[u][i]] = dist[u] + 1;
                pred[edges[u][i]] = u;
                queue.push_back(edges[u][i]);

                if (edges[u][i] == dest)
                    return true;
            }
        }
    }

    return false;
}

void Graph::printShortestDistance(int s, int dest) {
    int pred[numVertices + 1], dist[numVertices + 1];
    if (s == dest and s < numVertices) {
        cout << "1" << endl;
    } else if (!(bfs(s, dest, numVertices + 1, pred, dist))) {
        cerr << "Error: Given source and destination are not connected" << endl;
        return;
    } else {
        vector<int> path;
        int crawl = dest;
        path.push_back(crawl);
        while (pred[crawl] != -1) {
            path.push_back(pred[crawl]);
            crawl = pred[crawl];
        }

        for (int i = path.size() - 1; i > 0; i--) {
            cout << path[i] << "-";
        }
        cout << path[0] << "\n";
    }


}

void Graph::deleteEdge() {
    if (edges != NULL) {
        delete[] edges;
        edges = NULL;
    }
}


int size = 0;
Graph g;

void input_parser(string const &str, const char delim,
                  vector<string> &out) {
    string content;


    // create a stream from the string
    stringstream s(str);

    string s2;
    while (getline(s, s2, delim)) {
        out.push_back(s2); // store the string in s2
    }

    if (out.size() < 1) {
        cerr << "Error: Invalid input" << endl;
    } else if (out.size() == 2 && out[0] == "V") {
        string value(out.at(1));
        stringstream(value) >> size;
    } else if (out.size() == 2 && out[0] == "E") {
        g.refreshGraph(size);
        string &value(out.at(1));
        regex re("\\{(<[0-9]+,[0-9]+>[,]?)+\\}");
        smatch sm;
        regex_match(value, sm, re);
        if (sm[0] == value) {
            regex re1("<([0-9]+),([0-9]+)>");
            string::const_iterator iterStart = value.begin();
            string::const_iterator iterEnd = value.end();
            sregex_iterator iter(iterStart, iterEnd, re1);
            sregex_iterator end;
            int x_val = 0;
            int y_val = 0;
            int bCode = 0;
            while (iter != end) {
                if (bCode == 1) {
                    break;
                }
                for (unsigned i = 0; i < iter->size(); ++i) {
                    if (i == 1) {
                        x_val = stoi((*iter)[i].str());
                    }
                    if (i == 2) {
                        y_val = stoi((*iter)[i].str());
                    }
                }

                try {
                    if (x_val <= size && x_val > 0 && y_val <= size && y_val > 0) {
                        g.addEdge(x_val, y_val);
                    } else {
                        bCode = 1;
                        throw "Error: Invalid edge input";
                    }
                } catch (const char *errMessage) {
                    cerr << errMessage << endl;
                }
                ++iter;
            }


        } else {
            cerr << "Error: Invalid edge input" << endl;
        }
    } else if (out.size() == 3 && out[0] == "s") {
        int startNode = stoi(out[1]);
        int endNode = stoi(out[2]);
        g.printShortestDistance(startNode, endNode);
    } else {
        cerr << "Error: Invalid input" << endl;
    }
}


int main() {
    string s2;
    try {
        while (true) {
            getline(cin, s2);

            if (cin.eof()) {
                break;
            }

            const char delim = ' '; /* define the delimiter like space (' '), comma (,), hyphen (-), etc. */

            vector<string> out; // store the string in vector
            input_parser(s2, delim, out); // call function to split the string
        }
    }
    catch (const char *errMessage) {
        cerr << errMessage << endl;
    }
}


