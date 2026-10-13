#include "cut.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

Cut::Cut()
    : edge_count_(0) {
}

Cut::Cut(int edge_count)
    : edge_count_(0) {
    resize(edge_count);
}

void Cut::clear() {
    std::fill(selected_.begin(), selected_.end(), 0);
    edges_.clear();
}

void Cut::resize(int edge_count) {
    if (edge_count < 0) {
        throw std::invalid_argument("edge count cannot be negative");
    }

    edge_count_ = edge_count;
    selected_.assign(edge_count_, 0);
    edges_.clear();
}

void Cut::addEdge(int edge_id) {
    validateEdgeId(edge_id);

    if (selected_[edge_id] == 0) {
        selected_[edge_id] = 1;
        edges_.push_back(edge_id);
    }
}

void Cut::removeEdge(int edge_id) {
    validateEdgeId(edge_id);

    if (selected_[edge_id] == 0) {
        return;
    }

    selected_[edge_id] = 0;

    auto it = std::find(edges_.begin(), edges_.end(), edge_id);

    if (it != edges_.end()) {
        edges_.erase(it);
    }
}

bool Cut::contains(int edge_id) const {
    validateEdgeId(edge_id);
    return selected_[edge_id] != 0;
}

bool Cut::empty() const {
    return edges_.empty();
}

std::size_t Cut::size() const {
    return edges_.size();
}

int Cut::edgeCount() const {
    return edge_count_;
}

const std::vector<int>& Cut::edges() const {
    return edges_;
}

std::vector<int> Cut::edgeList() const {
    return edges_;
}

bool Cut::isSubsetOf(const Cut& other) const {
    if (edge_count_ != other.edge_count_) {
        throw std::invalid_argument("cuts must have the same edge universe");
    }

    for (int edge_id : edges_) {
        if (!other.contains(edge_id)) {
            return false;
        }
    }

    return true;
}

bool Cut::containsSubset(const Cut& candidate) const {
    if (edge_count_ != candidate.edge_count_) {
        throw std::invalid_argument("cuts must have the same edge universe");
    }

    return candidate.isSubsetOf(*this);
}

bool Cut::operator==(const Cut& other) const {
    if (edge_count_ != other.edge_count_) {
        return false;
    }

    if (edges_.size() != other.edges_.size()) {
        return false;
    }

    return std::equal(
        edges_.begin(),
        edges_.end(),
        other.edges_.begin()
    );
}

bool Cut::operator!=(const Cut& other) const {
    return !(*this == other);
}

std::string Cut::toString() const {
    std::ostringstream output;

    output << "{";

    for (std::size_t i = 0; i < edges_.size(); ++i) {
        if (i > 0) {
            output << ", ";
        }

        output << edges_[i];
    }

    output << "}";

    return output.str();
}

void Cut::validateEdgeId(int edge_id) const {
    if (edge_id < 0 || edge_id >= edge_count_) {
        throw std::out_of_range("invalid edge id");
    }
}