#include <iostream>
#include <vector>
#include <regex>
#include <string>
#include <list>
#include <minisat/core/Solver.h>

using namespace std;
using namespace Minisat;

class Graph {
    int numVertices;

public:

    void refreshGraph(int v);

    vector<int> getVertexCover(int size, int numVertexCover, vector<int> vertices);

};

void Graph::refreshGraph(int v) {
    numVertices = v;
}



vector<int> Graph::getVertexCover(int size, int numVertexCover, std::vector<int> vertices) {

    unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
    vector<Lit> propositions;
    vector<int> src;
    vector<int> des;
    vector<int> unsat;
    unsat.push_back(-1);

    for (int n = 0; n < size; ++n) {
        for (int k = 0; k < numVertexCover; ++k) {
            Lit literal = mkLit(solver->newVar());
            propositions.push_back(literal);
        }
    }

    for (int k = 0; k < numVertexCover; ++k) {
        vec<Lit> literals;
        for (int n = 0; n < size; ++n) { // n is changing first
            literals.push(propositions[n * (numVertexCover) + k]);
        }
        solver->addClause(literals);
        literals.clear();
    }

    for (int n = 0; n < size; ++n) {
        for (int i = 0; i < numVertexCover - 1; ++i) {
            for (int j = i + 1; j < numVertexCover; ++j) {
                solver->addClause(~propositions[n * (numVertexCover) + i],
                                  ~propositions[n * (numVertexCover) + j]);
            }
        }
    }

    for (int k = 0; k < numVertexCover; ++k) {
        for (int i = 0; i < size - 1; ++i) {
            for (int j = i + 1; j < size; ++j) {
                solver->addClause(~propositions[i * (numVertexCover) + k],
                                  ~propositions[j * (numVertexCover) + k]);
            }
        }
    }

    for (int i = 0; i < vertices.size(); ++i) {
        if (i % 2 == 0) {
            src.push_back(vertices[i]);
        } else {
            des.push_back(vertices[i]);
        }
    }

    for (int i = 0; i < src.size(); ++i) {
        vec<Lit> literals;
        for (int k = 0; k < numVertexCover; ++k) {
            literals.push(propositions[src[i] * (numVertexCover) + k]);
            literals.push(propositions[des[i] * (numVertexCover) + k]);
        }
        solver->addClause(literals);
        literals.clear();
    }

    bool isSat;
    isSat = solver->solve();

    if (isSat) {
        vector<int> result;
        for (int n = 0; n < size; n++) {
            for (int k = 0; k < numVertexCover; ++k) {
                if (toInt(solver->modelValue(propositions[n * (numVertexCover) + k])) == 0) {
                    result.push_back(n);
                }
            }
        }
//        solver.reset(new Minisat::Solver);
        return result;
    }
    solver.reset(new Minisat::Solver);
    return unsat;
}

int size = 0;
Graph g;


void input_parser(string const &str, const char delim,
                  vector<string> &out) {
    string content;
    vector<int> vertices;
    bool isValid = true;

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
                        vertices.push_back(x_val - 1);
                        vertices.push_back(y_val - 1);
                    } else {
                        bCode = 1;
                        isValid = false;
                        throw "Error: Invalid edge input";
                    }
                } catch (const char *errMessage) {
                    cerr << errMessage << endl;
                }
                ++iter;
            }

            if (isValid) {
                int initial = 1;
                int final = size;
                int mid;
                vector<int> satRes;
                vector<int> satRes1;
                vector<int> finalSatRes;
                vector<int> unsat;
                unsat.push_back(-1);

                while (initial <= final) {
                    mid = floor((initial + final) / 2);
                    satRes = g.getVertexCover(size, mid, vertices);
                    int count = 0;
                    for (int res: satRes) {
                        if (res == unsat[0]){
                            count ++;
                        }
                    }
                    if (count == 0) {

                        final = mid - 1;
                        finalSatRes.clear();
                        finalSatRes = satRes;

                    } else {
                        initial = mid + 1;
                    }
                    count = 0;
                }

                sort(finalSatRes.begin(), finalSatRes.end());

                for (int vertex: finalSatRes) {
                    cout << vertex + 1 << " ";
                }
                cout << "" << endl;
            }
        } else {
            cerr << "Error: Invalid edge input" << endl;
        }
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

            const char delim = ' ';

            vector<string> out;
            input_parser(s2, delim, out);
        }
    }
    catch (const char *errMessage) {
        cerr << errMessage << endl;
    }
}


