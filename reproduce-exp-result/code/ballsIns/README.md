# README #

## File Organization ##

* **ballsIns.cpp**: main function, a sample program
* **insertPart.h, insertPart.cpp**: implementation of truss maintenance
* **myG.h, myG.cpp**: a class for graph management
* **file.h, file.cpp**: implementation of file I/O
* **common.h, graph.h**: some global definition
* **tool/**: some useful tools

## How to Use the Code? ##

* First, use "index.out" to construct an index for the graph before update.
* Then, using this index as input, you can insert star motifs and get the new index.
* Example:
*       ./ballsIns.out indexFile starFile queryFile
* Or:
*       ./ballsIns.out indexFile starFile queryFile newIndexFile
* "indexFile" is the index. "starFile" contains star motifs. "queryFile" contains the center node of star motifs. "newIndexFile" is the output file.

## Data Format ##

* The file "starFile" contains star motifs is of the following format:

    n  c_1

    c_1 v_1

    c_1 v_2

    ...

    n  c_c

    c_2 u_1

    c_2 u_2

    ...

where n is a character, (c_i, v_j) is the edge of star motifs.

* The file "queryFile" containing the center node of star motifs is of the following format:

    c_1 0

    c_2 0

    ...

where (c_i, 0) means the center node c_i and the 0 here is temporarily useless.
