#include "statistics.h"

#include <sstream>

Statistics::Statistics()
    : candidates_(0),
      minimalCuts_(0),
      connectivityTests_(0),
      minimalityTests_(0),
      repositoryQueries_(0),
      prunedBranches_(0),
      branches_(0),
      elapsedMilliseconds_(0.0) {
}

void Statistics::reset() {
    candidates_ = 0;
    minimalCuts_ = 0;
    connectivityTests_ = 0;
    minimalityTests_ = 0;
    repositoryQueries_ = 0;
    prunedBranches_ = 0;
    branches_ = 0;
    elapsedMilliseconds_ = 0.0;
}

void Statistics::incrementCandidates(std::size_t count) {
    candidates_ += count;
}

void Statistics::incrementMinimalCuts(std::size_t count) {
    minimalCuts_ += count;
}

void Statistics::incrementConnectivityTests(std::size_t count) {
    connectivityTests_ += count;
}

void Statistics::incrementMinimalityTests(std::size_t count) {
    minimalityTests_ += count;
}

void Statistics::incrementRepositoryQueries(std::size_t count) {
    repositoryQueries_ += count;
}

void Statistics::incrementPrunedBranches(std::size_t count) {
    prunedBranches_ += count;
}

void Statistics::incrementBranches(std::size_t count) {
    branches_ += count;
}

std::size_t Statistics::getCandidates() const {
    return candidates_;
}

std::size_t Statistics::getMinimalCuts() const {
    return minimalCuts_;
}

std::size_t Statistics::getConnectivityTests() const {
    return connectivityTests_;
}

std::size_t Statistics::getMinimalityTests() const {
    return minimalityTests_;
}

std::size_t Statistics::getRepositoryQueries() const {
    return repositoryQueries_;
}

std::size_t Statistics::getPrunedBranches() const {
    return prunedBranches_;
}

std::size_t Statistics::getBranches() const {
    return branches_;
}

void Statistics::setElapsedMilliseconds(double milliseconds) {
    if (milliseconds < 0.0) {
        elapsedMilliseconds_ = 0.0;
        return;
    }

    elapsedMilliseconds_ = milliseconds;
}

double Statistics::getElapsedMilliseconds() const {
    return elapsedMilliseconds_;
}

double Statistics::getElapsedSeconds() const {
    return elapsedMilliseconds_ / 1000.0;
}

std::string Statistics::toString() const {
    std::ostringstream output;

    output << "Candidates: " << candidates_ << '\n';
    output << "Minimal cuts: " << minimalCuts_ << '\n';
    output << "Connectivity tests: " << connectivityTests_ << '\n';
    output << "Minimality tests: " << minimalityTests_ << '\n';
    //output << "Repository queries: " << repositoryQueries_ << '\n';
    //output << "Pruned branches: " << prunedBranches_ << '\n';
    output << "Branches: " << branches_ << '\n';
    //output << "Elapsed time (ms): "
    //       << elapsedMilliseconds_ << '\n';
    output << "Elapsed time (s): "
           << getElapsedSeconds();

    return output.str();
}