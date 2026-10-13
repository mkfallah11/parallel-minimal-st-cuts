#include "../src/graph.h"

#include "../src/solvers/sequential_solver.h"

#include "../src/utils/configuration.h"

#include <cassert>
#include <iostream>

int main() {
    Graph graph;

    graph.addEdge(0, 1);
    graph.addEdge(0, 2);
    graph.addEdge(0, 3);
    graph.addEdge(1, 2);
    graph.addEdge(1, 3);
    graph.addEdge(2, 3);

    Configuration configuration;

    configuration.setNumThreads(1);
    configuration.setEnableStatistics(true);
    configuration.setEnableTiming(true);
    configuration.setVerbose(true);

    SequentialSolver solver(
        graph,
        configuration
    );


    const std::vector<Cut>& cuts = 
        solver.solve();

    std::cout
        << "K4 minimal cuts: "
        << cuts.size()
        << '\n';

    assert(cuts.size() == 7);

    std::cout
        << "test_k4: PASS\n";

    return 0;
}