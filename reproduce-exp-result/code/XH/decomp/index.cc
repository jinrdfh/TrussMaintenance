#include "decomp.h"

int main(int argc, char** argv) {
    // read the graph and truss-decompose it
    ASSERT_MSG(3 == argc, "wrong parameters");
    truss_maint::decomp::Decomp index(argv[1]);
    index.WriteToFile(argv[2]);
}
