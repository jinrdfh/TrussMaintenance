# README #

## File Organization ##

* **star.cpp**: main function, implementation of dividing a graph into star motifs

## How to Use the Code? ##

* First, use "make" to get an executable file "star.out".
* Then, use "./star.out GraphFile StarFile" to convert the graph file "GraphFile" to the file of star motifs "StarFile".

## Data Format ##

* The graph file is of the following format:

    u_1 v_1

    u_2 v_2

    ...

    u_m v_m

where m is the # of edges, (u_i, v_i) is the i-th edge.

* The output file containing star motifs is of the following format:

    n c_1

    c_1 u_1

    c_1 u_2

    ...

    n c_2

    c_2 v_1

    c_2 v_2

    ...

where n is the character.
