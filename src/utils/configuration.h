#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <cstddef>
#include <string>

class Configuration {
public:
    Configuration();

    void reset();

    void setSource(int source);
    void setTarget(int target);

    int getSource() const;
    int getTarget() const;

    void setNumThreads(int numThreads);
    int getNumThreads() const;

    void setMaxDepth(std::size_t maxDepth);
    std::size_t getMaxDepth() const;

    void setEnableStatistics(bool enabled);
    bool isStatisticsEnabled() const;

    void setEnableTiming(bool enabled);
    bool isTimingEnabled() const;

    void setVerbose(bool enabled);
    bool isVerbose() const;

    void setInputFile(const std::string& inputFile);
    const std::string& getInputFile() const;

    void setOutputFile(const std::string& outputFile);
    const std::string& getOutputFile() const;

    std::string toString() const;

private:
    int source_;
    int target_;

    int numThreads_;

    std::size_t maxDepth_;

    bool enableStatistics_;
    bool enableTiming_;
    bool verbose_;

    std::string inputFile_;
    std::string outputFile_;
};

#endif