// Basic types for a weighted digraph.
#pragma once

#define CHECK(x)                                                            \
    do { if (!(x)) {                                                        \
            std::cerr <<"CHECK failed: "<< #x <<"\n"                        \
                      <<" at: " << __FILE__ <<":" << __LINE__ << "\n"       \
                      << " in function: " << __func__ << "\n"               \
                      << std::flush;                                        \
            std::abort();                                                   \
        } } while (0)

#include <cassert>
#include <cstdint>
#include <limits>
#include <string>
#include <iostream>

#include "uint_index.hh"
#include "saturated_uint.hh"

namespace ch {

// nodes are indexes of arrays
struct _node; 
using node = uint_index<std::uint_least32_t, _node>;  

// length of an edge (also weight or cost)
using edge_len = saturated_uint <std::uint_least32_t>;

// a distance is always obtained as a sum of edge lengths of a path
using dist = edge_len; 

constexpr std::uint_least32_t dist_max =
    std::numeric_limits<std::uint_least32_t>::max();

struct edge_head {
    node dst;
    edge_len len;
    edge_head(node dst, edge_len len) : dst(dst), len(len) {}
    operator node() const { return dst; }
    node head() const { return dst; }
    edge_len length() const { return len; }
    friend std::ostream& operator<<(std::ostream & os, edge_head hd) {
        os << hd.dst << "," << hd.len;
        return os;
    }
    bool operator<(const edge_head o) const {
        if (dst != o.dst) return dst < o.dst;
        return len < o.len;
    }
    bool operator==(const edge_head o) const {
        return dst == o.dst && len == o.len;
    }
};

struct edge : public edge_head {
    node src;
    edge(node src, edge_head hd) : edge_head(hd), src(src) {}
    edge(node src, node dst, edge_len len = 1u)
        : edge_head(dst, len), src(src) {}
    node tail() const { return src; }
    edge backward() const { return edge(dst, src, len); }
    friend std::ostream& operator<<(std::ostream& os, edge e) {
        os << "{" << e.src << ", " << e.dst << ", " << e.len << "}";
        return os;
    }
    bool operator<(const edge o) const {
        if (src != o.src) return src < o.src;
        if (dst != o.dst) return dst < o.dst;
        return len < o.len;
    }
    bool operator==(const edge o) const {
        return src == o.src && dst == o.dst && len == o.len;
    }
};

}

