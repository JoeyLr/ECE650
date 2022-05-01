#include <iostream>
#include <random>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>

using namespace std;


int randnGen(int max, int min) {
    unsigned int random;

    int fd = -1;

    if ((fd = open("/dev/urandom", O_RDONLY)) == -1) {

        cerr << "cannot open /dev/urandom." << endl;

        exit(1);

    }

    read(fd, &random, sizeof(unsigned int));

    close(fd);

    return min + (random % (max - min + 1));
}

string streetNameGen(unsigned length) {
    string alphabet = "abcdefghijklmnopqrstuvwxyz";
    string street_name;
    unsigned pos;
    while (street_name.size() != length) {
        pos = (rand() % (alphabet.size() - 1));
        street_name += alphabet.substr(pos, 1);
    }
    street_name += " street";
    return street_name;
}

bool isSamePoint(int x1, int y1, int x2, int y2) {
    if (x1 == x2 && y1 == y2) {
        return true;
    } else {
        return false;
    }
}

bool validPoint(int source_x, int source_y, int destination_x, int destination_y, vector<int> pointList) {
    if (isSamePoint(source_x, source_y, destination_x, destination_y)) {
        return false;
    } else if (pointList.size() == 0) {
        return true;
    } else if (pointList.size() == 2) {
        if (isSamePoint(source_x, source_y, pointList[0], pointList[1]) ||
            isSamePoint(destination_x, destination_y, pointList[0], pointList[1])) {
            return false;
        }
        return true;
    } else {
        for (unsigned i = 0; i < pointList.size(); i = i + 2) {
            if (isSamePoint(source_x, source_y, pointList[i], pointList[i + 1]) ||
                isSamePoint(destination_x, destination_y, pointList[i], pointList[i + 1])) {
                return false;
            }
        }
        return true;
    }
}


bool checkOverlap(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3) {
    if (x0 != x1 && x2 != x3) {
        float k1 = (y1 - y0) / (x1 - x0);
        float k2 = (y3 - y2) / (x3 - x2);
        if (k1 == k2) {
            if (y0 == y1 && y2 == y3) {
                if (y0 == y2 && (x0 > min(x2, x3) || x0 < max(x2, x3))) {
                    return true;
                } else {
                    return false;
                }
            } else {
                float distance = abs(y0 * (x1 - x0) - x0 * (y1 - y0) - y2 * (x3 - x2) + x2 * (y3 - y2));
                if (distance == 0) {
                    if ((x2 > min(x0, x1) && x2 < max(x0, x1) && y2 > min(y0, y1) && y2 < max(y0, y1)) ||
                        (x3 > min(x0, x1) && x3 < max(x0, x1) && y3 > min(y0, y1) && y3 < max(y0, y1))
                        || (x0 > min(x2, x3) && x0 < max(x2, x3) && y0 > min(y2, y3) && y0 < max(y2, y3)) ||
                        (x1 > min(x2, x3) && x1 < max(x2, x3) && y1 > min(y2, y3) && y1 < max(y2, y3))) {
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }

            }
        } else {
            return false;
        }
    } else {
        if (x0 == x1 && x2 == x3) {
            if (x0 == x2 && (y0 > min(y2, y3) || y0 < max(y2, y3))) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
}

bool isOverlap(vector<vector<int>> allStreetList, vector<int> lineList, int source_x, int source_y, int destination_x,
               int destination_y) {
    if (lineList.size() != 0) {
        for (unsigned i = 0; i < lineList.size() - 2; i = i + 2) {
            if (checkOverlap(source_x, source_y, destination_x, destination_y, lineList[i], lineList[i + 1],
                             lineList[i + 2], lineList[i + 3])) {
                return true;
            }

        }
        if (allStreetList.size() != 0) {
            for (auto &i: allStreetList) {
                for (unsigned j = 0; j < i.size() - 2; j = j + 2) {
                    if (checkOverlap(source_x, source_y, destination_x, destination_y, i[j],
                                     i[j + 1],
                                     i[j + 2], i[j + 3])) {
                        return true;
                    }
                }
            }
        }
    } else {
        if (allStreetList.size() != 0) {
            for (auto &i: allStreetList) {
                for (unsigned j = 0; j < i.size() - 2; j = j + 2) {
                    if (checkOverlap(source_x, source_y, destination_x, destination_y, i[j],
                                     i[j + 1],
                                     i[j + 2], i[j + 3])) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}


int main(int argc, char *argv[]) {
    srand((unsigned int) time(NULL));
    int street_k = 10;
    int line_k = 5;
    int wait_k = 5;
    int range_k = 20;

    for (int i = 1; i < argc - 1; i++) {
        char *pchar = argv[i];
        char *pchar1 = argv[i + 1];

        switch (pchar[0]) {
            case '-': {
                switch (pchar[1]) {
                    case 's': {
                        street_k = atoi(pchar1);
                        if (street_k < 2) {
                            cerr << "Error: Invalid Argument" << endl;
                        }
                        break;
                    }
                    case 'n': {
                        line_k = atoi(pchar1);
                        if (line_k < 1) {
                            cerr << "Error: Invalid Argument" << endl;
                        }
                        break;
                    }
                    case 'l': {
                        wait_k = atoi(pchar1);
                        if (wait_k < 5) {
                            cerr << "Error: Invalid Argument" << endl;
                        }
                        break;
                    }
                    case 'c': {
                        range_k = atoi(pchar1);
                        if (range_k < 1) {
                            cerr << "Error: Invalid Argument" << endl;
                        }
                        break;
                    }
                }
            }
            default:
                cout << "";
        }

    }
    int numStreet;
    int numLine;
    int numWait;
    int street_num = 0;
    vector<vector<int>> all_street_list;
    vector<string> street_list;
    int numAttempt = 0;

    while (true) {
        numStreet = randnGen(street_k, 2);
//            cout << "street number: " << numStreet << endl;
        while (street_num < numStreet) {
            vector<int> lineList;
            numLine = randnGen(line_k, 1);
            int numLine_loop = numLine;
//                cout << "line number: " << numLine << endl;
            for (int i = 0; i < numLine_loop; i++) {
                int src_x = randnGen(range_k, -range_k);
                int src_y = randnGen(range_k, -range_k);
                int des_x = randnGen(range_k, -range_k);
                int des_y = randnGen(range_k, -range_k);

                bool isValid = validPoint(src_x, src_y, des_x, des_y, lineList);
                bool whetherOverlap = !isOverlap(all_street_list, lineList, src_x, src_y, des_x, des_y);
                if (isValid && whetherOverlap) {
                    lineList.push_back(src_x);
                    lineList.push_back(src_y);
                    lineList.push_back(des_x);
                    lineList.push_back(des_y);
                    numAttempt = 0;
                } else {
                    numAttempt++;
                    if (numAttempt >= 25) {
                        cerr << "Error: failed to generate valid input for 25 simultaneous attempts" << endl;
                        exit(1);
                    }
                    numLine_loop++;
                }
            }
            all_street_list.push_back(lineList);


            street_num++;
            cout << "add ";
            string streetName = streetNameGen(4);
            street_list.push_back(streetName);

            cout << '"' << streetName << '"' << " ";
            if (lineList.size() == 2) {
                cout << "(" << lineList[0] << "," << lineList[1] << ")" << endl;
            } else if (lineList.size() == 4) {
                cout << "(" << lineList[0] << "," << lineList[1] << ") ";
                cout << "(" << lineList[2] << "," << lineList[3] << ")" << endl;
            } else {
                for (unsigned i = 0; i < lineList.size(); i = i + 2) {
                    if (i == lineList.size() - 2) {
                        cout << "(" << lineList[i] << "," << lineList[i + 1] << ")" << endl;
                    } else {
                        cout << "(" << lineList[i] << "," << lineList[i + 1] << ") ";
                    }
                }
            }
        }
        cout << "gg" << endl;
        street_num = 0;
        numWait = randnGen(wait_k, 5);
        sleep(numWait);

        for (auto &i: street_list) {
            std::cout << "rm " << '"' << i << '"' << std::endl;
        }
        street_list.clear();
    }

    return 0;
}

