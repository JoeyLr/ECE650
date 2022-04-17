#include<vector>
#include<iostream>
#include<unistd.h>
#include<sys/wait.h>
#include<cstdlib>

using namespace std;

int main (int argc, char **argv) {
    vector<pid_t> masterpro;
    opterr = 0;
    string temp;

    int status;
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


    char *rgenargv[10];

    rgenargv[0] = (char *) "./rgen";
    rgenargv[1] = (char *) "-s";
    rgenargv[2] = (char *) to_string(street_k).c_str();
    rgenargv[3] = (char *) "-n";
    rgenargv[4] = (char *) to_string(line_k).c_str();
    rgenargv[5] = (char *) "-l";
    rgenargv[6] = (char *) to_string(wait_k).c_str();
    rgenargv[7] = (char *) "-c";
    rgenargv[8] = (char *) to_string(range_k).c_str();
    rgenargv[9] = (char *) nullptr;

    char *pyargv[3];
    pyargv[0] = (char *) "/usr/bin/python3";
    pyargv[1] = (char *) "ece650-a1.py";
    pyargv[2] = nullptr;

    int cppToPy[2];
    pipe(cppToPy);

    int pyToCpp[2];
    pipe(pyToCpp);

    pid_t cppProcess;
    pid_t pyProcess;
    pid_t cppProcess2;

    pipe(cppToPy);
    pipe(pyToCpp);

    // 1st child
    cppProcess = fork();
    if (cppProcess == 0) {

        dup2(cppToPy[1], 1);
        close(cppToPy[0]);
        close(cppToPy[1]);

        execv("./rgen", rgenargv);
    } else if (cppProcess < 0) {
        cerr << "Error: could not fork" << endl;
    }
    masterpro.push_back(cppProcess);

    // 2nd child
    pyProcess = fork();
    if (pyProcess == 0) {

        dup2(cppToPy[0], 0);
        close(cppToPy[1]);
        close(cppToPy[0]);

        dup2(pyToCpp[1], 1);
        close(pyToCpp[0]);
        close(pyToCpp[1]);

        execv("/usr/bin/python3", pyargv);

    } else if (cppProcess < 0) {
        cerr << "Error: could not fork" << endl;
    }

    masterpro.push_back(pyProcess);

    // 3rd child
    cppProcess2 = fork();
    if (cppProcess2 == 0) {

        dup2(pyToCpp[0], 0);
        close(pyToCpp[1]);
        close(pyToCpp[0]);

        execlp("./ece650-a2", "./ece650-a2", nullptr);

    } else if (cppProcess2 < 0) {
        cerr << "could not fork" << endl;
    }

    masterpro.push_back(cppProcess2);

    dup2(pyToCpp[1], 1);
    close(pyToCpp[0]);
    close(pyToCpp[1]);

    while(true){
        string input;
        getline(cin, input);
        if (cin.eof()){
            break;
        }
        if (input.size() > 0)
            cout << input << endl;
    }

    for (pid_t childpro : masterpro)
    {
        kill (childpro, SIGTERM);
        waitpid(childpro, &status, 0);
    }


    return 0;
}