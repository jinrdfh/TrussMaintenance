/***************
main cpp
the code for truss decomposition, 
also can build the index for our maintenance algorithms in paper 
"Efficient Star-based Truss Maintenance on Dynamic Graphs".
****************/
#include "decomp.h"

/*****************
input:
        graph data file name
        output index file
description:
        main function
******************/
int main(int argc, char** argv) {
    // read the graph and truss-decompose it
    ASSERT_MSG(1 < argc, "wrong parameters");
    truss_maint::decomp::Decomp index(argv[1]);
    if (3 == argc)
    {
        index.WriteToFile(argv[2]);
    }
}
