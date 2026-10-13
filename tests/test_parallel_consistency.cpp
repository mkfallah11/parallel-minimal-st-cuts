#include "../src/graph.h"

#include "../src/solvers/sequential_solver.h"
#include "../src/solvers/sections_solver.h"
#include "../src/solvers/task_solver.h"
#include "../src/solvers/dynamic_solver.h"
#include "../src/solvers/work_queue_solver.h"
#include "../src/solvers/adaptive_solver.h"

#include "../src/utils/configuration.h"

#include <cassert>
#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace {

std::set<std::string> normalize(
    const std::vector<Cut>& cuts
) {
    std::set<std::string> result;

    for (const Cut& cut : cuts) {
        result.insert(
            cut.toString()
        );
    }

    return result;
}

void assertSame(
    const std::vector<Cut>& reference,
    const std::vector<Cut>& candidate
) {
    assert(
        normalize(reference) ==
        normalize(candidate)
    );
}

Graph makeK4() {
    Graph graph;

    graph.addEdge(0, 1);
    graph.addEdge(0, 2);
    graph.addEdge(0, 3);
    graph.addEdge(1, 2);
    graph.addEdge(1, 3);
    graph.addEdge(2, 3);

    return graph;
}

Graph makeTriangle() {
    Graph graph;

    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(2, 0);

    return graph;
}

Graph makePath4() {
    Graph graph;

    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);

    return graph;
}

void testGraph(
    const Graph& graph,
    int threads
) {
    Configuration configuration;

    configuration.setNumThreads(threads);
    configuration.setEnableStatistics(true);
    configuration.setEnableTiming(true);
    configuration.setVerbose(true);

    SequentialSolver sequential(
        graph,
        configuration
    );

    const std::vector<Cut>& reference = 
        sequential.solve();

    SectionsSolver sections(
        graph,
        configuration
    );

    TaskSolver tasks(
        graph,
        configuration
    );

    DynamicSolver dynamic(
        graph,
        configuration
    );

    WorkQueueSolver queue(
        graph,
        configuration
    );

    AdaptiveSolver adaptive(
        graph,
        configuration
    );

    assertSame(
        reference,
        sections.solve()
    );

    assertSame(
        reference,
        tasks.solve()
    );

    assertSame(
        reference,
        dynamic.solve()
    );

    assertSame(
        reference,
        queue.solve()
    );

    assertSame(
        reference,
        adaptive.solve()
    );
}

} // namespace

int main() {
    const std::vector<int> threadCounts = {
        1,
        2,
        4
    };

    const Graph k4 = makeK4();
    const Graph triangle = makeTriangle();
    const Graph path4 = makePath4();

    for (int threads : threadCounts) {
        testGraph(
            k4,
            threads
        );

        testGraph(
            triangle,
            threads
        );

        testGraph(
            path4,
            threads
        );
    }

    std::cout
        << "test_parallel_consistency: PASS\n";

    return 0;
}