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

#include "saturated_int.hh"

// forward declare friend operator
//namespace ch { struct edge; }
//std::ostream& operator<<(std::ostream& os, ch::edge e) ;

namespace ch {

using node = std::uint_least32_t;  // nodes are indexes of arrays

// length of an edge (also weight or cost)
using edge_len = saturated_int <std::uint_least32_t>; 

using dist = edge_len; // sum of edge lengths of a path

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
};

}

