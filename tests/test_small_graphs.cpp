#include "../src/graph.h"

#include "../src/solvers/sequential_solver.h"

#include "../src/utils/configuration.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

struct GraphTest {
    std::string name;
    Graph graph;
    std::size_t expectedCuts;
};

int main() {
    Configuration configuration;

    configuration.setNumThreads(1);
    configuration.setEnableStatistics(true);
    configuration.setEnableTiming(true);
    configuration.setVerbose(true);

    std::vector<GraphTest> tests;

    {
        Graph graph;

        graph.addEdge(0, 1);

        tests.push_back({
            "K2",
            graph,
            1
        });
    }

    {
        Graph graph;

        graph.addEdge(0, 1);
        graph.addEdge(1, 2);

        tests.push_back({
            "Path3",
            graph,
            1
        });
    }

    {
        Graph graph;

        graph.addEdge(0, 1);
        graph.addEdge(1, 2);
        graph.addEdge(2, 3);

        tests.push_back({
            "Path4",
            graph,
            1
        });
    }

    {
        Graph graph;

        graph.addEdge(0, 1);
        graph.addEdge(1, 2);
        graph.addEdge(2, 0);

        tests.push_back({
            "Triangle",
            graph,
            3
        });
    }

    {
        Graph graph;

        graph.addEdge(0, 1);
        graph.addEdge(0, 2);
        graph.addEdge(0, 3);
        graph.addEdge(1, 2);
        graph.addEdge(1, 3);
        graph.addEdge(2, 3);

        tests.push_back({
            "K4",
            graph,
            7
        });
    }

    for (const GraphTest& test : tests) {
        SequentialSolver solver(
            test.graph,
            configuration
        );

        const std::vector<Cut>& cuts = solver.solve();

        std::cout
            << test.name
            << ": expected="
            << test.expectedCuts
            << ", actual="
            << cuts.size()
            << '\n';

        assert(
            cuts.size() ==
            test.expectedCuts
        );
    }

    std::cout
        << "test_small_graphs: PASS\n";

    return 0;
}