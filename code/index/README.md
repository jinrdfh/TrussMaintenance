# README #

## File Organization ##

* **decomp.h, decomp.cc**: implementation of truss decomposition
* **index.cc**: main function, a sample program for truss decomposition

## How to Use the Code? ##

* First, use "make" to get an executable file "index.out".
* Then, use "./index.out GraphFile indexFile" to build the index "indexFile" of the graph "GraphFile".

## Data Format ##

* The graph file is of the following format:

    u_1 v_1

    u_2 v_2

    ...

    u_m v_m

where m is the # of edges, (u_i, v_i) is the i-th edge.

