#include "mcv_state.h"

#include <algorithm>

MCVState::MCVState()
    : source_(0),
      target_(0),
      depth_(0),
      S_(),
      T_(),
      B_(),
      N_(),
      A_(),
      D_() {
}

MCVState::MCVState(int edge_count)
    : source_(0),
      target_(0),
      depth_(0),
      S_(),
      T_(),
      B_(),
      N_(),
      A_(edge_count),
      D_(edge_count) {
}

void MCVState::clear() {
    source_ = 0;
    target_ = 0;
    depth_ = 0;

    S_.clear();
    T_.clear();
    B_.clear();
    N_.clear();

    A_.clear();
    D_.clear();
}

void MCVState::initialize(
    int edge_count,
    int source,
    int target
) {
    source_ = source;
    target_ = target;
    depth_ = 0;

    S_.clear();
    T_.clear();
    B_.clear();
    N_.clear();

    A_.resize(edge_count);
    D_.resize(edge_count);
}

int MCVState::source() const {
    return source_;
}

int MCVState::target() const {
    return target_;
}

int MCVState::depth() const {
    return depth_;
}

void MCVState::setSource(int source) {
    source_ = source;
}

void MCVState::setTarget(int target) {
    target_ = target;
}

void MCVState::setDepth(int depth) {
    depth_ = depth;
}

std::vector<int>& MCVState::S() {
    return S_;
}

const std::vector<int>& MCVState::S() const {
    return S_;
}

std::vector<int>& MCVState::T() {
    return T_;
}

const std::vector<int>& MCVState::T() const {
    return T_;
}

std::vector<int>& MCVState::B() {
    return B_;
}

const std::vector<int>& MCVState::B() const {
    return B_;
}

std::vector<int>& MCVState::N() {
    return N_;
}

const std::vector<int>& MCVState::N() const {
    return N_;
}

void MCVState::setS(
    const std::vector<int>& vertices
) {
    S_ = vertices;
}

void MCVState::setT(
    const std::vector<int>& vertices
) {
    T_ = vertices;
}

void MCVState::setB(
    const std::vector<int>& vertices
) {
    B_ = vertices;
}

void MCVState::setN(
    const std::vector<int>& vertices
) {
    N_ = vertices;
}

Cut& MCVState::A() {
    return A_;
}

const Cut& MCVState::A() const {
    return A_;
}

Cut& MCVState::D() {
    return D_;
}

const Cut& MCVState::D() const {
    return D_;
}

void MCVState::setA(const Cut& cut) {
    A_ = cut;
}

void MCVState::setD(const Cut& cut) {
    D_ = cut;
}

void MCVState::addToS(int vertex) {
    addUnique(S_, vertex);
}

void MCVState::removeFromS(int vertex) {
    removeValue(S_, vertex);
}

void MCVState::addToT(int vertex) {
    addUnique(T_, vertex);
}

void MCVState::removeFromT(int vertex) {
    removeValue(T_, vertex);
}

void MCVState::addToB(int vertex) {
    addUnique(B_, vertex);
}

void MCVState::removeFromB(int vertex) {
    removeValue(B_, vertex);
}

void MCVState::addToN(int vertex) {
    addUnique(N_, vertex);
}

void MCVState::removeFromN(int vertex) {
    removeValue(N_, vertex);
}

bool MCVState::containsInS(int vertex) const {
    return containsValue(S_, vertex);
}

bool MCVState::containsInT(int vertex) const {
    return containsValue(T_, vertex);
}

bool MCVState::containsInB(int vertex) const {
    return containsValue(B_, vertex);
}

bool MCVState::containsInN(int vertex) const {
    return containsValue(N_, vertex);
}

MCVState MCVState::makeChild() const {
    MCVState child(*this);

    child.depth_ = depth_ + 1;

    return child;
}

void MCVState::addUnique(
    std::vector<int>& container,
    int value
) {
    if (!containsValue(container, value)) {
        container.push_back(value);
    }
}

void MCVState::removeValue(
    std::vector<int>& container,
    int value
) {
    auto it = std::find(
        container.begin(),
        container.end(),
        value
    );

    if (it != container.end()) {
        container.erase(it);
    }
}

bool MCVState::containsValue(
    const std::vector<int>& container,
    int value
) {
    return std::find(
        container.begin(),
        container.end(),
        value
    ) != container.end();
}