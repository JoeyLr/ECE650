#include <iostream>
#include <utility>
#include <vector>
#include <regex>
#include <string>
#include <list>
#include <minisat/core/Solver.h>
#include <pthread.h>
#include <chrono>

using namespace std;
using namespace Minisat;
static Minisat::Solver *solver;

#define RUNNING 1
#define WAITING 0
#define ENDING 2
#define DEBUG std::getenv("DEBUG") != NULL
vector <int>* edges;
vector <int> vertices;
class Graph {
    int numVertices{};
    vector<int> *edges;
    vector<int> graphVertices;
public:

    Graph() : edges(nullptr) {}

    void refreshGraph(int v);

    void resetVertices(vector<int> vertices);

    vector<int> getVertices();

    void addEdge(int x, int y);

    void deleteEdge();

    vector<int> *getEdge();
};

void Graph::resetVertices(vector<int> v) {
    graphVertices = move(v);
}

vector<int> Graph::getVertices() {
    return graphVertices;
}

void Graph::refreshGraph(int v) {
    deleteEdge();
    numVertices = v;
    edges = new vector<int>[numVertices + 1];
}

void Graph::addEdge(int x, int y) {
    edges[x].push_back(y);
    edges[y].push_back(x);
}

vector<int> *Graph::getEdge() {
    return edges;
}

void Graph::deleteEdge() {
    if (edges != nullptr) {
        delete[] edges;
        edges = nullptr;
    }
}

vector<int> getVertexCover(int size, int numVertexCover, vector<int> vertices) {
    solver = new Minisat::Solver();
    vector<Lit> propositions;
    vector<int> src;
    vector<int> des;

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
                    result.push_back(n + 1);
                }
            }
        }
        delete solver;
        return result;
    }
    delete solver;
    return {-1};
}

vector<int> CNF_SAT_VC(int size, const vector<int>& vertices) {
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
        satRes = getVertexCover(size, mid, vertices);
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
    return finalSatRes;
}

int findHighestDegree(int numVertices, vector<int> *edges, vector<int> traversedVertices, vector<int> traverseEdges) {
    int highestDegree = 0;
    int highestDegreeVertex = 0;
    bool isOverlap = false;
    for (int i = 1; i < numVertices + 1; i++) {
        if (find(traversedVertices.begin(), traversedVertices.end(), i) == traversedVertices.end()) {
            unsigned long degree = 0;
            for (int vertex: edges[i]) {
                for (int j = 0; j < traverseEdges.size(); j = j + 2) {
                    if ((traverseEdges[j] == vertex && traverseEdges[j + 1] == i) || (traverseEdges[j] == i && traverseEdges[j + 1] == vertex)) {
                        isOverlap = true;
                    }
                }
                if (!isOverlap){
                    degree ++;
                }
                isOverlap = false;
            if (degree > highestDegree) {
                highestDegree = degree;
                highestDegreeVertex = i;
            }
            }

        }
    }
//    cout << highestDegree << endl;
    return highestDegreeVertex;
}

vector<int> APPROX_VC_2(int size, vector<int> *edges, vector<int> vertices) {
    vector<int> result;
    vector<int> traversedEdges;
    vector<int> traverseVertices;
    while (traversedEdges.size() < vertices.size()) {
        for (int j = 0; j < vertices.size(); j = j + 2) {
            if (traversedEdges.empty() ||
                (find(traverseVertices.begin(), traverseVertices.end(), vertices[j] + 1) == traverseVertices.end() &&
                 find(traverseVertices.begin(), traverseVertices.end(), vertices[j + 1] + 1) ==
                 traverseVertices.end())) {
                result.push_back(vertices[j] + 1);
                result.push_back(vertices[j + 1] + 1);
                traversedEdges.push_back(vertices[j] + 1);
                traversedEdges.push_back(vertices[j + 1] + 1);
                traverseVertices.push_back(vertices[j] + 1);
                traverseVertices.push_back(vertices[j + 1] + 1);

//                for (int i=0; i < size + 1; i++) {
//                    cout << "vertex " << i << endl;
//                    for (int v: edges[i]){
//                        cout << v << " ";
//                    }
//                    cout << "" << endl;
//                }

                for (int vertex: edges[vertices[j] + 1]) {
                    if (vertex != vertices[j + 1] + 1 && (find(traverseVertices.begin(), traverseVertices.end(), vertex) == traverseVertices.end())){
                        traversedEdges.push_back(vertices[j] + 1);
                        traversedEdges.push_back(vertex);
                    }
                }

                for (int vertex: edges[vertices[j + 1] + 1]) {
                    if (vertex != vertices[j] + 1 && (find(traverseVertices.begin(), traverseVertices.end(), vertex) == traverseVertices.end())) {
                        traversedEdges.push_back(vertices[j + 1] + 1);
                        traversedEdges.push_back(vertex);
                    }
                }
            }
        }
        sort(result.begin(), result.end());
    }
    return result;
}

vector<int> APPROX_VC_1(int size, vector<int> *edges, vector<int> vertices) {
    vector<int> traversedVertices;
    vector<int> result;
    vector<int> traversedEdges;
    while (traversedEdges.size() < vertices.size()) {
        int vertexHighestDegree = findHighestDegree(size, edges, traversedVertices, traversedEdges);
        traversedVertices.push_back(vertexHighestDegree);
//        for (int vertex: edges[vertexHighestDegree]) {
//            if (find(traversedVertices.begin(), traversedVertices.end(), vertex) == traversedVertices.end()) {
//                traversedVertices.push_back(vertex);
//            }
//        }
        for (int vertex: edges[vertexHighestDegree]) {
            if (find(traversedVertices.begin(), traversedVertices.end(), vertex) == traversedVertices.end()) {
                traversedEdges.push_back(vertexHighestDegree);
                traversedEdges.push_back(vertex);
            }
        }
        result.push_back(vertexHighestDegree);
    }
    sort(result.begin(), result.end());
    return result;
}

int size = 0;
Graph g;
pthread_t threadIO, thread_CNF_SAT_VC, thread_APPROX_VC_2, thread_APPROX_VC_1;
vector<int> res_CNF_SAT_VC, res_APPROX_VC_1, res_APPROX_VC_2;
int thread_s_CNF_SAT_VC = WAITING;
bool timeout_CNF_SAT_VC = false;
long double thread_t_CNF_SAT_VC, thread_t_CNF_SAT_VC_1, thread_t_APPROX_VC_1, thread_t_APPROX_VC_2;

static long double getClocktime(clockid_t cid)
{
    struct timespec ts{};
    if (clock_gettime(cid, &ts) == -1)
        cerr << "Error: can't get the time of clock" << endl;
    return (ts.tv_sec / 1000000) + (ts.tv_nsec / 1000);
}

clockid_t getCPUClockID(pthread_t thread_id)
{
    clockid_t cid;
    if (pthread_getcpuclockid(thread_id, &cid))
    {
        cerr << "Error: fail to get cpu clock id" << endl;
    }
    return cid;
}

void isTimeOut(pthread_t thread_id, int time_limit)
{
//    cout << "isTimeOut start" << endl;
    clockid_t cid = getCPUClockID(thread_id);
    struct timespec timespec{};
    while (thread_s_CNF_SAT_VC != ENDING)
    {
        clock_gettime(cid, &timespec);
        if (timespec.tv_sec > time_limit)
        {
                timeout_CNF_SAT_VC = true;
//            solver->interrupt();
//            if (solver->verbosity > 0)
//            {
//                solver->printStats();
//            }
            return;
        }
    }
}

void *APPROX_VC_1_thread(void *arg) {
//    cout << "APPROX_VC_1 start" << endl;
    clockid_t cid = getCPUClockID(pthread_self());
    auto start_time = getClocktime(cid);

    res_APPROX_VC_1 = APPROX_VC_1(size, g.getEdge(), g.getVertices());

    auto end_time = getClocktime(cid);
    thread_t_APPROX_VC_1 = end_time - start_time;

    return nullptr;
}

void *APPROX_VC_2_thread(void *arg) {
//    cout << "APPROX_VC_2 start" << endl;
    clockid_t cid = getCPUClockID(pthread_self());
    auto start_time = getClocktime(cid);

    res_APPROX_VC_2 = APPROX_VC_2(size, g.getEdge(), g.getVertices());
    auto end_time = getClocktime(cid);

    thread_t_APPROX_VC_2 = end_time - start_time;

    return nullptr;
}

void *CNF_SAT_VC_thread(void *arg) {

//    cout << "CNF_SAT_VC start" << endl;

    clockid_t cid = getCPUClockID(pthread_self());
    thread_s_CNF_SAT_VC = RUNNING;
    auto start_time = getClocktime(cid);

    res_CNF_SAT_VC = CNF_SAT_VC(size, g.getVertices());

    auto end_time = getClocktime(cid);
    thread_t_CNF_SAT_VC = end_time - start_time;
    thread_s_CNF_SAT_VC = ENDING;

    return nullptr;
}


void printResult(const vector<int> &res, const string &algorithm) {
    cout << algorithm << ": ";
    if (algorithm == "CNF_SAT_VC" && timeout_CNF_SAT_VC){
        cout << "timeout" <<endl;
    } else {
        for (int i = 0; i < res.size()-1; i++) {
            cout << res[i] << ",";
        }
        cout << res[res.size()-1] << endl;
    }
}

void *io_thread(void *arg) {
    string s1;
    while (true) {
        getline(cin, s1);
        if (cin.eof()) {
            exit(1);
        }
        const char delim = ' ';
        vector<string> out;
//        vector<int> vertices;
        bool isValid = true;

        stringstream s(s1);

        string s2;
        while (getline(s, s2, delim)) {
            out.push_back(s2);
        }

        if (out.empty()) {
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
                            g.addEdge(x_val, y_val);
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
                g.resetVertices(vertices);
                edges = g.getEdge();
                vertices = g.getVertices();

                if (isValid) {
                    pthread_create(&thread_CNF_SAT_VC, nullptr, &CNF_SAT_VC_thread, nullptr);
                    pthread_create(&thread_APPROX_VC_1, nullptr, &APPROX_VC_1_thread, nullptr);
                    pthread_create(&thread_APPROX_VC_2, nullptr, &APPROX_VC_2_thread, nullptr);

                    isTimeOut(thread_CNF_SAT_VC, 10);
//                    cout << "end" << endl;

                    if(timeout_CNF_SAT_VC) {
                        pthread_cancel(thread_CNF_SAT_VC);
                    } else {
                        pthread_join(thread_CNF_SAT_VC, nullptr);
                    }
                    pthread_join(thread_APPROX_VC_1, nullptr);
                    pthread_join(thread_APPROX_VC_2, nullptr);


                    printResult(res_CNF_SAT_VC, "CNF_SAT_VC");
                    printResult(res_APPROX_VC_1, "APPROX_VC_1");
                    printResult(res_APPROX_VC_2, "APPROX_VC_2");

                    timeout_CNF_SAT_VC = false;

                    DEBUG &&cout << "CNF-SAT-VC CPU time:   " << thread_t_CNF_SAT_VC << " µs" << endl;
                    DEBUG &&cout << "APPROX-VC-1 CPU time:   " << thread_t_APPROX_VC_1 << " µs" << endl;
                    DEBUG &&cout << "APPROX-VC-2 CPU time:   " << thread_t_APPROX_VC_2 << " µs" << endl;
                    vertices.clear();
                }
            } else {
                cerr << "Error: Invalid edge input" << endl;
            }
        } else {
            cerr << "Error: Invalid input" << endl;
        }
    }
}


int main() {
    int rc;
    try {
        rc = pthread_create(&threadIO, nullptr, &io_thread, nullptr);
        if (rc) return 0;

        rc = pthread_join(threadIO, nullptr);
        if (rc) return 0;
    }
    catch (const char *errMessage) {
        cerr << errMessage << endl;
    }
    return 0;
}




