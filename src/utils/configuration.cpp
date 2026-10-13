#include "configuration.h"

#include <sstream>

Configuration::Configuration()
    : source_(0),
      target_(1),
      numThreads_(1),
      maxDepth_(0),
      enableStatistics_(true),
      enableTiming_(true),
      verbose_(false),
      inputFile_(),
      outputFile_() {
}

void Configuration::reset() {
    source_ = 0;
    target_ = 1;

    numThreads_ = 1;

    maxDepth_ = 0;

    enableStatistics_ = true;
    enableTiming_ = true;
    verbose_ = false;

    inputFile_.clear();
    outputFile_.clear();
}

void Configuration::setSource(int source) {
    source_ = source;
}

void Configuration::setTarget(int target) {
    target_ = target;
}

int Configuration::getSource() const {
    return source_;
}

int Configuration::getTarget() const {
    return target_;
}

void Configuration::setNumThreads(int numThreads) {
    if (numThreads < 1) {
        numThreads_ = 1;
        return;
    }

    numThreads_ = numThreads;
}

int Configuration::getNumThreads() const {
    return numThreads_;
}

void Configuration::setMaxDepth(std::size_t maxDepth) {
    maxDepth_ = maxDepth;
}

std::size_t Configuration::getMaxDepth() const {
    return maxDepth_;
}

void Configuration::setEnableStatistics(bool enabled) {
    enableStatistics_ = enabled;
}

bool Configuration::isStatisticsEnabled() const {
    return enableStatistics_;
}

void Configuration::setEnableTiming(bool enabled) {
    enableTiming_ = enabled;
}

bool Configuration::isTimingEnabled() const {
    return enableTiming_;
}

void Configuration::setVerbose(bool enabled) {
    verbose_ = enabled;
}

bool Configuration::isVerbose() const {
    return verbose_;
}

void Configuration::setInputFile(const std::string& inputFile) {
    inputFile_ = inputFile;
}

const std::string& Configuration::getInputFile() const {
    return inputFile_;
}

void Configuration::setOutputFile(const std::string& outputFile) {
    outputFile_ = outputFile;
}

const std::string& Configuration::getOutputFile() const {
    return outputFile_;
}

std::string Configuration::toString() const {
    std::ostringstream output;

    output << "Source: " << source_ << '\n';
    output << "Target: " << target_ << '\n';
    output << "Threads: " << numThreads_ << '\n';
    output << "Max depth: " << maxDepth_ << '\n';
    output << "Statistics: " << (enableStatistics_ ? "enabled" : "disabled") << '\n';
    output << "Timing: " << (enableTiming_ ? "enabled" : "disabled") << '\n';
    output << "Verbose: " << (verbose_ ? "enabled" : "disabled") << '\n';
    output << "Input file: " << inputFile_ << '\n';
    output << "Output file: " << outputFile_;

    return output.str();
}