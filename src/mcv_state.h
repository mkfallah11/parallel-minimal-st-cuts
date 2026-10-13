#ifndef MINIMAL_CUT_MCV_STATE_H
#define MINIMAL_CUT_MCV_STATE_H

#include "cut.h"

#include <vector>

class MCVState {
public:
    MCVState();
    explicit MCVState(int edge_count);

    void clear();
    void initialize(int edge_count, int source, int target);

    int source() const;
    int target() const;
    int depth() const;

    void setSource(int source);
    void setTarget(int target);
    void setDepth(int depth);

    std::vector<int>& S();
    const std::vector<int>& S() const;

    std::vector<int>& T();
    const std::vector<int>& T() const;

    std::vector<int>& B();
    const std::vector<int>& B() const;

    std::vector<int>& N();
    const std::vector<int>& N() const;

    void setS(const std::vector<int>& vertices);
    void setT(const std::vector<int>& vertices);
    void setB(const std::vector<int>& vertices);
    void setN(const std::vector<int>& vertices);

    Cut& A();
    const Cut& A() const;

    Cut& D();
    const Cut& D() const;

    void setA(const Cut& cut);
    void setD(const Cut& cut);

    void addToS(int vertex);
    void removeFromS(int vertex);

    void addToT(int vertex);
    void removeFromT(int vertex);

    void addToB(int vertex);
    void removeFromB(int vertex);

    void addToN(int vertex);
    void removeFromN(int vertex);

    bool containsInS(int vertex) const;
    bool containsInT(int vertex) const;
    bool containsInB(int vertex) const;
    bool containsInN(int vertex) const;

    MCVState makeChild() const;

private:
    int source_;
    int target_;
    int depth_;

    std::vector<int> S_;
    std::vector<int> T_;
    std::vector<int> B_;
    std::vector<int> N_;

    Cut A_;
    Cut D_;

    static void addUnique(
        std::vector<int>& container,
        int value
    );

    static void removeValue(
        std::vector<int>& container,
        int value
    );

    static bool containsValue(
        const std::vector<int>& container,
        int value
    );
};

#endif