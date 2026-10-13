#ifndef STATISTICS_H
#define STATISTICS_H

#include <cstddef>
#include <string>

class Statistics {
public:
    Statistics();

    void reset();

    void incrementCandidates(std::size_t count = 1);
    void incrementMinimalCuts(std::size_t count = 1);
    void incrementConnectivityTests(std::size_t count = 1);
    void incrementMinimalityTests(std::size_t count = 1);
    void incrementRepositoryQueries(std::size_t count = 1);
    void incrementPrunedBranches(std::size_t count = 1);
    void incrementBranches(std::size_t count = 1);

    std::size_t getCandidates() const;
    std::size_t getMinimalCuts() const;
    std::size_t getConnectivityTests() const;
    std::size_t getMinimalityTests() const;
    std::size_t getRepositoryQueries() const;
    std::size_t getPrunedBranches() const;
    std::size_t getBranches() const;

    void setElapsedMilliseconds(double milliseconds);
    double getElapsedMilliseconds() const;
    double getElapsedSeconds() const;

    std::string toString() const;

private:
    std::size_t candidates_;
    std::size_t minimalCuts_;
    std::size_t connectivityTests_;
    std::size_t minimalityTests_;
    std::size_t repositoryQueries_;
    std::size_t prunedBranches_;
    std::size_t branches_;

    double elapsedMilliseconds_;
};

#endif
