// Template class example for Mirror Bridge
// Templates must be explicitly instantiated before binding

#pragma once
#include <vector>
#include <numeric>

template<typename T>
struct Container {
    std::vector<T> data;

    void push(T value) {
        data.push_back(value);
    }

    T sum() const {
        return std::accumulate(data.begin(), data.end(), T{});
    }

    size_t size() const {
        return data.size();
    }

    void clear() {
        data.clear();
    }
};

// Explicit instantiations - required for binding
// These create concrete types that can be reflected
template struct Container<int>;
template struct Container<double>;
