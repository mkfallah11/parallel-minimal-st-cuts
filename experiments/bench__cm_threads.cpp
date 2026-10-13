#include "../src/graph.h"

#include "../src/solvers/sequential_solver.h"
#include "../src/solvers/sections_solver.h"
#include "../src/solvers/task_solver.h"
#include "../src/solvers/dynamic_solver.h"
#include "../src/solvers/work_queue_solver.h"
#include "../src/solvers/adaptive_solver.h"

#include "../src/utils/configuration.h"

#include <omp.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

const std::vector<int> threadCounts = {
    1, 2, 4, 8, 12, 16, 20
};

const std::vector<std::string> parallelSolverNames = {
    "OpenMP Sections",
    "OpenMP Tasks",
    "Dynamic Tasking",
    "Explicit Work Queue",
    "Adaptive"
};

struct SolverResult {
    std::string name;
    std::size_t cutCount = 0;
    double timeMilliseconds = 0.0;
};

struct FamilyKey {
    int n = 0;
    int d = 0;

    bool operator<(const FamilyKey& other) const
    {
        if (n != other.n) {
            return n < other.n;
        }

        return d < other.d;
    }
};

struct FamilyStatistics {
    int n = 0;
    int d = 0;

    int expectedGraphs = 100;
    int processedGraphs = 0;

    std::uint64_t totalEdges = 0;
    std::uint64_t totalCuts = 0;

    std::size_t minimumCuts =
        std::numeric_limits<std::size_t>::max();

    std::size_t maximumCuts = 0;

    // key: solver name, thread count
    std::map<std::pair<std::string, int>, double> totalTime;

    FamilyStatistics()
    {
        for (int threads : threadCounts) {
            totalTime[{"Sequential", threads}] = 0.0;

            for (const std::string& solverName :
                 parallelSolverNames) {
                totalTime[{solverName, threads}] = 0.0;
            }
        }
    }
};

struct ParsedGraphFile {
    int n = 0;
    int d = 0;
    int r = 0;
    std::filesystem::path path;
};

const std::regex graphFilePattern(
    R"(^Gnm_n(12|16|20|24)_d(04|06|08)_r([0-9]{3})\.txt$)"
);

bool parseGraphFile(
    const std::filesystem::path& path,
    ParsedGraphFile& result)
{
    const std::string filename = path.filename().string();
    std::smatch match;

    if (!std::regex_match(filename, match, graphFilePattern)) {
        return false;
    }

    result.n = std::stoi(match[1].str());
    result.d = std::stoi(match[2].str());
    result.r = std::stoi(match[3].str());
    result.path = path;

    return true;
}

template <typename Solver>
SolverResult runSolver(
    const Graph& graph,
    const Configuration& configuration,
    const std::string& solverName)
{
    Solver solver(graph, configuration);

    const auto start = std::chrono::steady_clock::now();
    std::vector<Cut> cuts = solver.solve();
    const auto end = std::chrono::steady_clock::now();

    SolverResult result;
    result.name = solverName;
    result.cutCount = cuts.size();
    result.timeMilliseconds =
        std::chrono::duration<double, std::milli>(
            end - start
        ).count();

    return result;
}

std::vector<SolverResult> runParallelSolvers(
    const Graph& graph,
    const Configuration& configuration)
{
    std::vector<SolverResult> results;

    results.push_back(
        runSolver<SectionsSolver>(
            graph, configuration, "OpenMP Sections"
        )
    );

    results.push_back(
        runSolver<TaskSolver>(
            graph, configuration, "OpenMP Tasks"
        )
    );

    results.push_back(
        runSolver<DynamicSolver>(
            graph, configuration, "Dynamic Tasking"
        )
    );

    results.push_back(
        runSolver<WorkQueueSolver>(
            graph, configuration, "Explicit Work Queue"
        )
    );

    results.push_back(
        runSolver<AdaptiveSolver>(
            graph, configuration, "Adaptive"
        )
    );

    return results;
}

void updateFamilyStatistics(
    FamilyStatistics& statistics,
    const Graph& graph,
    std::size_t cutCount,
    const std::vector<SolverResult>& results,
    int threads)
{
    statistics.processedGraphs++;

    statistics.totalEdges +=
        static_cast<std::uint64_t>(graph.edgeCount());

    statistics.totalCuts +=
        static_cast<std::uint64_t>(cutCount);

    statistics.minimumCuts =
        std::min(statistics.minimumCuts, cutCount);

    statistics.maximumCuts =
        std::max(statistics.maximumCuts, cutCount);

    for (const SolverResult& result : results) {
        statistics.totalTime[{result.name, threads}] +=
            result.timeMilliseconds;
    }
}

double averageEdges(const FamilyStatistics& statistics)
{
    if (statistics.processedGraphs == 0) {
        return 0.0;
    }

    return static_cast<double>(statistics.totalEdges) /
           statistics.processedGraphs;
}

double averageCuts(const FamilyStatistics& statistics)
{
    if (statistics.processedGraphs == 0) {
        return 0.0;
    }

    return static_cast<double>(statistics.totalCuts) /
           statistics.processedGraphs;
}

double averageTime(
    const FamilyStatistics& statistics,
    const std::string& solverName,
    int threads)
{
    if (statistics.processedGraphs == 0) {
        return 0.0;
    }

    return statistics.totalTime.at({solverName, threads}) /
           statistics.processedGraphs;
}

void writeTextHeader(
    std::ofstream& output,
    int maximumThreads)
{
    output
        << "RANDOM GRAPH FAMILY BENCHMARK\n"
        << "=============================\n\n"
        << "Maximum available threads: "
        << maximumThreads << '\n'
        << "Expected graphs per family: 100\n"
        << "Graph pattern: Gnm_nXX_dYY_rZZZ.txt\n"
        << "Thread counts: 1, 2, 4, 8, 12, 16, 20\n\n"
        << "Sequential always uses one thread.\n"
        << "Parallel solvers are tested with every listed thread count.\n\n";
}

void writeFamilyToText(
    std::ofstream& output,
    const FamilyStatistics& statistics)
{
    output
        << "============================================================\n"
        << "Family: n=" << statistics.n
        << ", d=" << std::setw(2) << std::setfill('0')
        << statistics.d << std::setfill(' ') << '\n'
        << "============================================================\n"
        << "Graphs processed: " << statistics.processedGraphs
        << " / " << statistics.expectedGraphs << "\n\n"
        << std::fixed << std::setprecision(3)
        << "Graph Statistics\n"
        << "----------------\n"
        << "Average edges:             " << averageEdges(statistics) << '\n'
        << "Average minimal cuts:      " << averageCuts(statistics) << '\n';

    if (statistics.processedGraphs > 0) {
        output
            << "Minimum minimal cuts:      " << statistics.minimumCuts << '\n'
            << "Maximum minimal cuts:      " << statistics.maximumCuts << '\n';
    }
    else {
        output
            << "Minimum minimal cuts:      N/A\n"
            << "Maximum minimal cuts:      N/A\n";
    }

    output
        << "\nAverage Solver Execution Time (ms)\n"
        << "----------------------------------\n"
        << std::left << std::setw(25) << "Solver"
        << std::right << std::setw(12) << "Threads"
        << std::setw(18) << "Average ms"
        << std::setw(18) << "Average s" << '\n'
        << std::string(73, '-') << '\n';

    output << std::fixed << std::setprecision(3);

    output
        << std::left << std::setw(25) << "Sequential"
        << std::right << std::setw(12) << 1
        << std::setw(18) << averageTime(statistics, "Sequential", 1)
        << std::setw(18) << averageTime(statistics, "Sequential", 1) / 1000.0
        << '\n';

    for (const std::string& solverName : parallelSolverNames) {
        for (int threads : threadCounts) {
            output
                << std::left << std::setw(25) << solverName
                << std::right << std::setw(12) << threads
                << std::setw(18)
                << averageTime(statistics, solverName, threads)
                << std::setw(18)
                << averageTime(statistics, solverName, threads) / 1000.0
                << '\n';
        }
    }

    output << '\n';
}

void writeCsvHeader(std::ofstream& output)
{
    output
        << "n,d,graphs_processed,average_edges,average_cuts,"
        << "minimum_cuts,maximum_cuts,solver,threads,average_ms,average_s\n";
}

void writeFamilyToCsv(
    std::ofstream& output,
    const FamilyStatistics& statistics)
{
    for (const std::string& solverName :
         std::vector<std::string>{"Sequential"}) {
        const int threads = 1;
        const double milliseconds =
            averageTime(statistics, solverName, threads);

        output
            << statistics.n << ','
            << statistics.d << ','
            << statistics.processedGraphs << ','
            << std::fixed << std::setprecision(3)
            << averageEdges(statistics) << ','
            << averageCuts(statistics) << ',';

        if (statistics.processedGraphs > 0) {
            output << statistics.minimumCuts << ','
                   << statistics.maximumCuts << ',';
        }
        else {
            output << "0,0,";
        }

        output
            << solverName << ','
            << threads << ','
            << milliseconds << ','
            << milliseconds / 1000.0 << '\n';
    }

    for (const std::string& solverName : parallelSolverNames) {
        for (int threads : threadCounts) {
            const double milliseconds =
                averageTime(statistics, solverName, threads);

            output
                << statistics.n << ','
                << statistics.d << ','
                << statistics.processedGraphs << ','
                << std::fixed << std::setprecision(3)
                << averageEdges(statistics) << ','
                << averageCuts(statistics) << ',';

            if (statistics.processedGraphs > 0) {
                output << statistics.minimumCuts << ','
                       << statistics.maximumCuts << ',';
            }
            else {
                output << "0,0,";
            }

            output
                << solverName << ','
                << threads << ','
                << milliseconds << ','
                << milliseconds / 1000.0 << '\n';
        }
    }
}

} // namespace

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr
            << "Usage:\n  "
            << argv[0]
            << " <graph-directory>\n";
        return 1;
    }

    try {
        const std::filesystem::path graphDirectory = argv[1];

        if (!std::filesystem::exists(graphDirectory) ||
            !std::filesystem::is_directory(graphDirectory)) {
            std::cerr
                << "Input path is not a valid directory: "
                << graphDirectory << '\n';
            return 1;
        }

        const int maximumThreads = omp_get_max_threads();

        if (maximumThreads < 20) {
            std::cout
                << "Warning: OpenMP reports a maximum of "
                << maximumThreads
                << " threads. Tests requesting 20 threads will still be attempted.\n";
        }

        omp_set_dynamic(0);

        std::map<FamilyKey, std::vector<ParsedGraphFile>> families;

        for (const auto& entry :
             std::filesystem::directory_iterator(graphDirectory)) {
            if (!entry.is_regular_file()) {
                continue;
            }

            ParsedGraphFile graphFile;

            if (!parseGraphFile(entry.path(), graphFile)) {
                continue;
            }

            if (graphFile.r < 1 || graphFile.r > 100) {
                continue;
            }

            families[{graphFile.n, graphFile.d}].push_back(graphFile);
        }

        if (families.empty()) {
            std::cerr << "No matching graph files were found.\n";
            return 1;
        }

        for (auto& family : families) {
            std::sort(
                family.second.begin(),
                family.second.end(),
                [](const ParsedGraphFile& a, const ParsedGraphFile& b) {
                    return a.r < b.r;
                }
            );
        }

        const auto textOutput =
            graphDirectory / "random_graph_family_benchmark.txt";

        const auto csvOutput =
            graphDirectory / "random_graph_family_benchmark.csv";

        std::ofstream output(textOutput);
        std::ofstream csv(csvOutput);

        if (!output || !csv) {
            std::cerr << "Failed to create output files.\n";
            return 1;
        }

        writeTextHeader(output, maximumThreads);
        writeCsvHeader(csv);

        std::cout
            << "Found " << families.size()
            << " graph families.\n";

        for (auto& familyEntry : families) {
            const FamilyKey key = familyEntry.first;
            const auto& graphFiles = familyEntry.second;

            FamilyStatistics statistics;
            statistics.n = key.n;
            statistics.d = key.d;

            std::cout
                << "\n==================================================\n"
                << "Family n=" << key.n
                << ", d=" << std::setw(2) << std::setfill('0')
                << key.d << std::setfill(' ')
                << "\nGraphs: " << graphFiles.size() << " / 100\n"
                << "==================================================\n";

            if (graphFiles.size() != 100) {
                std::cout
                    << "Warning: expected 100 graphs, found "
                    << graphFiles.size() << ".\n";
            }

            for (const ParsedGraphFile& graphFile : graphFiles) {
                std::cout
                    << "\nGraph "
                    << statistics.processedGraphs + 1
                    << '/' << graphFiles.size()
                    << ": " << graphFile.path.filename().string()
                    << '\n';

                Graph graph;

                if (!graph.loadFromFile(graphFile.path.string())) {
                    std::cerr
                        << "Failed to load graph: "
                        << graphFile.path << '\n';
                    continue;
                }

                try {
                    Configuration sequentialConfiguration;
                    sequentialConfiguration.setNumThreads(1);
                    sequentialConfiguration.setEnableStatistics(true);
                    sequentialConfiguration.setEnableTiming(true);
                    sequentialConfiguration.setVerbose(false);

                    const SolverResult sequentialResult =
                        runSolver<SequentialSolver>(
                            graph,
                            sequentialConfiguration,
                            "Sequential"
                        );

                    std::vector<SolverResult> graphResults;
                    graphResults.push_back(sequentialResult);

                    std::cout
                        << "  Sequential, threads=1: "
                        << sequentialResult.timeMilliseconds
                        << " ms, cuts="
                        << sequentialResult.cutCount << '\n';

                    for (int threads : threadCounts) {
                        Configuration configuration;
                        configuration.setNumThreads(threads);
                        configuration.setEnableStatistics(true);
                        configuration.setEnableTiming(true);
                        configuration.setVerbose(false);

                        omp_set_num_threads(threads);

                        std::cout
                            << "  Parallel solvers, threads="
                            << threads << '\n';

                        const std::vector<SolverResult> results =
                            runParallelSolvers(graph, configuration);

                        for (const SolverResult& result : results) {
                            graphResults.push_back(result);

                            std::cout
                                << "    " << result.name
                                << ": " << result.timeMilliseconds
                                << " ms\n";
                        }

                        updateFamilyStatistics(
                            statistics,
                            graph,
                            sequentialResult.cutCount,
                            results,
                            threads
                        );
                    }

                    // The graph-level edge and cut statistics are counted once.
                    // Parallel execution times are accumulated for each thread count.
                    statistics.totalEdges -=
                        static_cast<std::uint64_t>(graph.edgeCount());
                    statistics.totalCuts -=
                        static_cast<std::uint64_t>(sequentialResult.cutCount);
                    statistics.minimumCuts =
                        std::min(statistics.minimumCuts,
                                 sequentialResult.cutCount);
                    statistics.maximumCuts =
                        std::max(statistics.maximumCuts,
                                 sequentialResult.cutCount);

                    // Add the sequential execution time separately.
                    statistics.totalTime[{"Sequential", 1}] +=
                        sequentialResult.timeMilliseconds;

                }
                catch (const std::exception& exception) {
                    std::cerr
                        << "Solver error for "
                        << graphFile.path.filename().string()
                        << ": " << exception.what() << '\n';
                }
            }

            writeFamilyToText(output, statistics);
            writeFamilyToCsv(csv, statistics);

            std::cout
                << "\nFamily summary:\n"
                << "  Graphs processed: "
                << statistics.processedGraphs << '\n'
                << "  Average edges: "
                << std::fixed << std::setprecision(3)
                << averageEdges(statistics) << '\n'
                << "  Average cuts: "
                << averageCuts(statistics) << '\n'
                << "  Minimum cuts: "
                << (statistics.processedGraphs > 0
                        ? statistics.minimumCuts : 0)
                << '\n'
                << "  Maximum cuts: "
                << statistics.maximumCuts << '\n';
        }

        output
            << "============================================================\n"
            << "END OF BENCHMARK\n";

        std::cout
            << "\nBenchmark completed successfully.\n"
            << "Text result: " << textOutput << '\n'
            << "CSV result: " << csvOutput << '\n';

        return 0;
    }
    catch (const std::exception& exception) {
        std::cerr
            << "Error: " << exception.what() << '\n';
        return 1;
    }
}