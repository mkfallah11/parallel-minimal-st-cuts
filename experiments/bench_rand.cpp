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

    std::map<std::string, double> totalTime;

    FamilyStatistics()
    {
        totalTime["Sequential"] = 0.0;
        totalTime["OpenMP Sections"] = 0.0;
        totalTime["OpenMP Tasks"] = 0.0;
        totalTime["Dynamic Tasking"] = 0.0;
        totalTime["Explicit Work Queue"] = 0.0;
        totalTime["Adaptive"] = 0.0;
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
    const std::string filename =
        path.filename().string();

    std::smatch match;

    if (!std::regex_match(
            filename,
            match,
            graphFilePattern)) {

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

    const auto start =
        std::chrono::steady_clock::now();

    std::vector<Cut> cuts = solver.solve();

    const auto end =
        std::chrono::steady_clock::now();

    const double milliseconds =
        std::chrono::duration<double, std::milli>(
            end - start
        ).count();

    SolverResult result;

    result.name = solverName;
    result.cutCount = cuts.size();
    result.timeMilliseconds = milliseconds;

    return result;
}

std::vector<SolverResult> runAllSolvers(
    const Graph& graph,
    const Configuration& configuration)
{
    std::vector<SolverResult> results;

    std::cout
        << "  Running Sequential..."
        << std::endl;

    results.push_back(
        runSolver<SequentialSolver>(
            graph,
            configuration,
            "Sequential"
        )
    );

    std::cout
        << "  Running OpenMP Sections..."
        << std::endl;

    results.push_back(
        runSolver<SectionsSolver>(
            graph,
            configuration,
            "OpenMP Sections"
        )
    );

    std::cout
        << "  Running OpenMP Tasks..."
        << std::endl;

    results.push_back(
        runSolver<TaskSolver>(
            graph,
            configuration,
            "OpenMP Tasks"
        )
    );

    std::cout
        << "  Running Dynamic Tasking..."
        << std::endl;

    results.push_back(
        runSolver<DynamicSolver>(
            graph,
            configuration,
            "Dynamic Tasking"
        )
    );

    std::cout
        << "  Running Explicit Work Queue..."
        << std::endl;

    results.push_back(
        runSolver<WorkQueueSolver>(
            graph,
            configuration,
            "Explicit Work Queue"
        )
    );

    std::cout
        << "  Running Adaptive..."
        << std::endl;

    results.push_back(
        runSolver<AdaptiveSolver>(
            graph,
            configuration,
            "Adaptive"
        )
    );

    return results;
}

void updateFamilyStatistics(
    FamilyStatistics& statistics,
    const Graph& graph,
    const std::vector<SolverResult>& results)
{
    if (results.empty()) {
        return;
    }

    statistics.processedGraphs++;

    /*
     * use the sequential solver as the reference
     * for the number of minimal cuts.
     */
    const std::size_t cutCount =
        results.front().cutCount;

    statistics.totalEdges +=
        static_cast<std::uint64_t>(graph.edgeCount());

    statistics.totalCuts +=
        static_cast<std::uint64_t>(cutCount);

    statistics.minimumCuts =
        std::min(
            statistics.minimumCuts,
            cutCount
        );

    statistics.maximumCuts =
        std::max(
            statistics.maximumCuts,
            cutCount
        );

    for (const SolverResult& result : results) {
        statistics.totalTime[result.name] +=
            result.timeMilliseconds;
    }
}

double averageEdges(
    const FamilyStatistics& statistics)
{
    if (statistics.processedGraphs == 0) {
        return 0.0;
    }

    return static_cast<double>(
               statistics.totalEdges
           ) /
           static_cast<double>(
               statistics.processedGraphs
           );
}

double averageCuts(
    const FamilyStatistics& statistics)
{
    if (statistics.processedGraphs == 0) {
        return 0.0;
    }

    return static_cast<double>(
               statistics.totalCuts
           ) /
           static_cast<double>(
               statistics.processedGraphs
           );
}

double averageTime(
    const FamilyStatistics& statistics,
    const std::string& solverName)
{
    if (statistics.processedGraphs == 0) {
        return 0.0;
    }

    return statistics.totalTime.at(solverName) /
           static_cast<double>(
               statistics.processedGraphs
           );
}

void writeTextHeader(
    std::ofstream& output,
    int threads)
{
    output
        << "RANDOM GRAPH FAMILY BENCHMARK\n"
        << "=============================\n\n";

    output
        << "Threads: "
        << threads
        << "\n";

    output
        << "Expected graphs per family: 100\n";

    output
        << "Graph pattern: Gnm_nXX_dYY_rZZZ.txt\n\n";

    output
        << "Families:\n"
        << "  n = {12, 16, 20, 24}\n"
        << "  d = {04, 06, 08}\n"
        << "  r = {001, ..., 100}\n\n";

    output
        << "Statistics:\n"
        << "  Average number of edges\n"
        << "  Average number of minimal s-t cuts\n"
        << "  Minimum number of minimal s-t cuts\n"
        << "  Maximum number of minimal s-t cuts\n"
        << "  Average execution time of each solver\n\n";
}

void writeFamilyToText(
    std::ofstream& output,
    const FamilyStatistics& statistics)
{
    output
        << "============================================================\n";

    output
        << "Family: n="
        << statistics.n
        << ", d="
        << std::setw(2)
        << std::setfill('0')
        << statistics.d
        << std::setfill(' ')
        << "\n";

    output
        << "============================================================\n";

    output
        << "Graphs processed: "
        << statistics.processedGraphs
        << " / "
        << statistics.expectedGraphs
        << "\n\n";

    output
        << std::fixed
        << std::setprecision(3);

    output
        << "Graph Statistics\n";
    output
        << "----------------\n";

    output
        << "Average edges:             "
        << averageEdges(statistics)
        << "\n";

    output
        << "Average minimal cuts:      "
        << averageCuts(statistics)
        << "\n";

    if (statistics.processedGraphs > 0) {
        output
            << "Minimum minimal cuts:      "
            << statistics.minimumCuts
            << "\n";

        output
            << "Maximum minimal cuts:      "
            << statistics.maximumCuts
            << "\n";
    }
    else {
        output
            << "Minimum minimal cuts:      0\n";

        output
            << "Maximum minimal cuts:      0\n";
    }

    output << "\n";

    output
        << "Average Solver Execution Time\n";

    output
        << "-----------------------------\n";

    output
        << std::left
        << std::setw(28)
        << "Solver"
        << std::right
        << std::setw(20)
        << "Time (ms)"
        << std::setw(20)
        << "Time (s)"
        << "\n";

    output
        << std::string(68, '-')
        << "\n";

    const std::vector<std::string> solverNames = {
        "Sequential",
        "OpenMP Sections",
        "OpenMP Tasks",
        "Dynamic Tasking",
        "Explicit Work Queue",
        "Adaptive"
    };

    for (const std::string& solverName : solverNames) {
        const double milliseconds =
            averageTime(
                statistics,
                solverName
            );

        output
            << std::left
            << std::setw(28)
            << solverName
            << std::right
            << std::setw(20)
            << milliseconds
            << std::setw(20)
            << milliseconds / 1000.0
            << "\n";
    }

    output << "\n";
}

void writeCsvHeader(
    std::ofstream& output)
{
    output
        << "n,d,graphs_processed,"
        << "average_edges,"
        << "average_cuts,"
        << "minimum_cuts,"
        << "maximum_cuts,"
        << "sequential_ms,"
        << "sections_ms,"
        << "tasks_ms,"
        << "dynamic_ms,"
        << "work_queue_ms,"
        << "adaptive_ms\n";
}

void writeFamilyToCsv(
    std::ofstream& output,
    const FamilyStatistics& statistics)
{
    output
        << statistics.n
        << ','
        << statistics.d
        << ','
        << statistics.processedGraphs
        << ','
        << std::fixed
        << std::setprecision(3)
        << averageEdges(statistics)
        << ','
        << averageCuts(statistics)
        << ',';

    if (statistics.processedGraphs > 0) {
        output
            << statistics.minimumCuts
            << ','
            << statistics.maximumCuts
            << ',';
    }
    else {
        output
            << "0,0,";
    }

    output
        << averageTime(
               statistics,
               "Sequential"
           )
        << ','
        << averageTime(
               statistics,
               "OpenMP Sections"
           )
        << ','
        << averageTime(
               statistics,
               "OpenMP Tasks"
           )
        << ','
        << averageTime(
               statistics,
               "Dynamic Tasking"
           )
        << ','
        << averageTime(
               statistics,
               "Explicit Work Queue"
           )
        << ','
        << averageTime(
               statistics,
               "Adaptive"
           )
        << '\n';
}

} // namespace

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr
            << "Usage:\n"
            << "  "
            << argv[0]
            << " <graph-directory>\n";

        return 1;
    }

    try {
        const std::filesystem::path graphDirectory =
            argv[1];

        if (!std::filesystem::exists(graphDirectory)) {
            std::cerr
                << "Graph directory does not exist: "
                << graphDirectory
                << '\n';

            return 1;
        }

        if (!std::filesystem::is_directory(graphDirectory)) {
            std::cerr
                << "Input path is not a directory: "
                << graphDirectory
                << '\n';

            return 1;
        }

        const int threads =
            omp_get_max_threads();

        Configuration configuration;

        configuration.setNumThreads(threads);
        configuration.setEnableStatistics(true);
        configuration.setEnableTiming(true);
        configuration.setVerbose(false);

        std::map<
            FamilyKey,
            std::vector<ParsedGraphFile>
        > families;

        for (const auto& entry :
             std::filesystem::directory_iterator(
                 graphDirectory)) {

            if (!entry.is_regular_file()) {
                continue;
            }

            ParsedGraphFile graphFile;

            if (!parseGraphFile(
                    entry.path(),
                    graphFile)) {

                continue;
            }

            if (graphFile.r < 1 ||
                graphFile.r > 100) {

                continue;
            }

            FamilyKey key;

            key.n = graphFile.n;
            key.d = graphFile.d;

            families[key].push_back(
                graphFile
            );
        }

        if (families.empty()) {
            std::cerr
                << "No matching graph files were found.\n";

            return 1;
        }

        for (auto& family : families) {
            std::sort(
                family.second.begin(),
                family.second.end(),
                [](const ParsedGraphFile& a,
                   const ParsedGraphFile& b) {

                    return a.r < b.r;
                }
            );
        }

        const std::filesystem::path textOutput =
            graphDirectory /
            "random_graph_family_benchmark.txt";

        const std::filesystem::path csvOutput =
            graphDirectory /
            "random_graph_family_benchmark.csv";

        std::ofstream output(textOutput);

        if (!output) {
            std::cerr
                << "Failed to create result file: "
                << textOutput
                << '\n';

            return 1;
        }

        std::ofstream csv(csvOutput);

        if (!csv) {
            std::cerr
                << "Failed to create CSV result file: "
                << csvOutput
                << '\n';

            return 1;
        }

        writeTextHeader(
            output,
            threads
        );

        writeCsvHeader(csv);

        std::cout
            << "Found "
            << families.size()
            << " graph families.\n\n";

        for (auto& familyEntry : families) {
            const FamilyKey& key =
                familyEntry.first;

            std::vector<ParsedGraphFile>& graphFiles =
                familyEntry.second;

            FamilyStatistics statistics;

            statistics.n = key.n;
            statistics.d = key.d;

            std::cout
                << "==================================================\n"
                << "Family n="
                << key.n
                << ", d="
                << std::setw(2)
                << std::setfill('0')
                << key.d
                << std::setfill(' ')
                << "\n"
                << "Graphs: "
                << graphFiles.size()
                << " / 100\n"
                << "==================================================\n";

            if (graphFiles.size() != 100) {
                std::cout
                    << "Warning: expected 100 graphs, found "
                    << graphFiles.size()
                    << ".\n";
            }

            for (const ParsedGraphFile& graphFile :
                 graphFiles) {

                std::cout
                    << "\n["
                    << statistics.processedGraphs + 1
                    << "/"
                    << graphFiles.size()
                    << "] "
                    << graphFile.path.filename().string()
                    << '\n';

                Graph graph;

                if (!graph.loadFromFile(
                        graphFile.path.string())) {

                    std::cerr
                        << "Failed to load graph: "
                        << graphFile.path
                        << '\n';

                    continue;
                }

                try {
                    const std::vector<SolverResult> results =
                        runAllSolvers(
                            graph,
                            configuration
                        );

                    updateFamilyStatistics(
                        statistics,
                        graph,
                        results
                    );

                    if (!results.empty()) {
                        std::cout
                            << "  Minimal cuts: "
                            << results.front().cutCount
                            << '\n';
                    }
                }
                catch (const std::exception& exception) {
                    std::cerr
                        << "Solver error for "
                        << graphFile.path.filename().string()
                        << ": "
                        << exception.what()
                        << '\n';
                }
            }

            writeFamilyToText(
                output,
                statistics
            );

            writeFamilyToCsv(
                csv,
                statistics
            );

            std::cout
                << "\nFamily summary:\n"
                << "  Average edges: "
                << std::fixed
                << std::setprecision(3)
                << averageEdges(statistics)
                << '\n'
                << "  Average cuts: "
                << averageCuts(statistics)
                << '\n'
                << "  Minimum cuts: "
                << (
                    statistics.processedGraphs > 0
                        ? statistics.minimumCuts
                        : 0
                )
                << '\n'
                << "  Maximum cuts: "
                << statistics.maximumCuts
                << '\n';
        }

        output
            << "============================================================\n"
            << "END OF BENCHMARK\n";

        output.close();
        csv.close();

        std::cout
            << "\nBenchmark completed successfully.\n"
            << "Text result: "
            << textOutput
            << '\n'
            << "CSV result: "
            << csvOutput
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