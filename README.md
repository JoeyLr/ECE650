# Traffic-Cameras-Management-with-Vertex-Cover
In this project, minimum vertex cover problem will be attempted to minimize the number of cameras but still monitor the traffic situation efficiently.

## File Structure

```
Traffic-Cameras-Management-with-Vertex-Cover
│     
│
└─── shortest_path
│   │   CMakeLists.txt
│   │   graph_generator.py  // generate graph with given street coordinates
│   │   shortest_path.cpp  // find the shortest path of two vertex
│   │   rgen.cpp  // randomly generate valid input of graph_generator.py
│   │   main.cpp  // run graph_generator.py, shortest_path.cpp and main.cpp with multi-processes
│   
└─── vertex_cover
    │   CMakeLists.txt
    │   vertex_cover.cpp // solve the minimum vertex-cover with three methods
```

## Shortest Path

To run the code, using folowwing command
```
cd build && cmake ../ && make install && cd ./run/bin && ./main

V 10
E {<1,5>,<5,10>,<4,7>,<10,4>}
s 1 10
1-5-10

```

## Vertex Cover

CNF-SAT-VC method solves the satisfiability problem with the open-source tool minisat (http://minisat.se/)
before reproduce the code, clone the github project of minisat with 
```
git clone https://github.com/niklasso/minisat
```

To run the code, using folowwing command
```
cd build && cmake ../ && make && ./vertex_cover

V 5
E {<3,2>,<3,1>,<3,4>,<2,5>,<5,4>}

CNF-SAT-VC: 3,5
APPROX-VC-1: 3,5
APPROX-VC-2: 1,3,4,5

```
