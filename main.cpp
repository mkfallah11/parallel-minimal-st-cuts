#include "src/graph.h"

#include "src/solvers/sequential_solver.h"
#include "src/solvers/sections_solver.h"
#include "src/solvers/task_solver.h"
#include "src/solvers/dynamic_solver.h"
#include "src/solvers/work_queue_solver.h"
#include "src/solvers/adaptive_solver.h"

#include "src/utils/configuration.h"
#include "src/utils/statistics.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void printUsage(const char* program) {
    std::cout
        << "Usage:\n"
        << "  " << program << " <graph-file> [solver] [threads]\n\n"
        << "Solvers:\n"
        << "  seq\n"
        << "  sections\n"
        << "  tasks\n"
        << "  dynamic\n"
        << "  queue\n"
        << "  adaptive\n";
}

void printCuts(const std::vector<Cut>& cuts) {
    std::cout << "Cuts:\n";

    for (std::size_t i = 0; i < cuts.size(); ++i) {
        std::cout << "  " << i << ": "
                  << cuts[i].toString()
                  << '\n';
    }
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    try {
        const std::string graphFile = argv[1];

        const std::string solverName =
            argc >= 3 ? argv[2] : "seq";

        const int threads =
            argc >= 4 ? std::stoi(argv[3]) : 1;

        Graph graph;

        if (!graph.loadFromFile(graphFile)) {
            std::cerr
                << "Failed to load graph: "
                << graphFile
                << '\n';

            return 1;
        }

        Configuration configuration;

        configuration.setNumThreads(threads);
        configuration.setEnableStatistics(true);
        configuration.setEnableTiming(true);
        configuration.setVerbose(true);

        std::vector<Cut> cuts;

        if (solverName == "seq") {
            SequentialSolver solver(
                graph,
                configuration
            );

            cuts = solver.solve();

            std::cout << "Solver: Sequential\n";
            std::cout << solver.getStatistics().toString();
        }
        else if (solverName == "sections") {
            SectionsSolver solver(
                graph,
                configuration
            );

            cuts = solver.solve();

            std::cout << "Solver: OpenMP Sections\n";
            std::cout << solver.getStatistics().toString();
        }
        else if (solverName == "tasks") {
            TaskSolver solver(
                graph,
                configuration
            );

            cuts = solver.solve();

            std::cout << "Solver: OpenMP Tasks\n";
            std::cout << solver.getStatistics().toString();
        }
        else if (solverName == "dynamic") {
            DynamicSolver solver(
                graph,
                configuration
            );

            cuts = solver.solve();

            std::cout << "Solver: Dynamic Tasking\n";
            std::cout << solver.getStatistics().toString();
        }
        else if (solverName == "queue") {
            WorkQueueSolver solver(
                graph,
                configuration
            );

            cuts = solver.solve();

            std::cout << "Solver: Explicit Work Queue\n";
            std::cout << solver.getStatistics().toString();
        }
        else if (solverName == "adaptive") {
            AdaptiveSolver solver(
                graph,
                configuration
            );

            cuts = solver.solve();

            std::cout << "Solver: Adaptive\n";
            std::cout << solver.getStatistics().toString();
        }
        else {
            std::cerr
                << "Unknown solver: "
                << solverName
                << '\n';

            printUsage(argv[0]);
            return 1;
        }

        std::cout
            << "\nVertices: "
            << graph.vertexCount()
            << '\n';

        std::cout
            << "Edges: "
            << graph.edgeCount()
            << '\n';

        std::cout
            << "Minimal cuts: "
            << cuts.size()
            << '\n';

        return 0;
    }
    catch (const std::exception& exception) {
        std::cerr
            << "Error: "
            << exception.what()
            << '\n';

        return 1;
    }
}