// Author: Laurent Viennot, Inria, 2020.

#include <sstream>
#include <fstream>

#include "digraph.hh"

// ----------------- digraph : a graph as a vector of vectors


digraph::hrange digraph::out_neighbors(node u) const {
    assert(u >= 0 && u < _n);
    return hrange(out_neighb[u].cbegin(), out_neighb[u].cend());
}

std::ostream& operator<<(std::ostream & os, const digraph & g) {
    os << "{ ";
    bool first = true;
    for (node u : g) {
        for (auto e : g[u]) {
            if (first) { first = false; }
            else { os << ",\n  "; }
            os << edge(u, e);
        }
    }
    os << " }\n";
    return os;
}

std::istream& operator>>(std::istream & is, digraph & g) {
    
    std::string src_s, dst_s, len_s, line;
    const node node_max = std::numeric_limits<node>::max();
    const edge_len length_max = std::numeric_limits<edge_len>::max();
    
    while ( ! is.eof()) { //is.peek() != std::ifstream::traits_type::eof()) {
        is >> std::ws;
        if (is.eof()) return is;
        std::getline(is, line);
        while (line.size() > 0 && line[0] == '#') {
            if (is.eof()) return is;
            std::getline(is, line);
        }
        std::istringstream iss(line);
        CHECK( ! iss.eof());
        iss >> src_s;
        CHECK( ! iss.eof());
        iss >> dst_s;
        CHECK( ! iss.eof());
        iss >> len_s;
        iss >> std::ws;
        CHECK(iss.eof());
        auto src_i = std::stoi(src_s);
        auto dst_i = std::stoi(dst_s);
        auto len_i = std::stoi(len_s);
        CHECK(src_i >= 0 && uint64_t(src_i) <= uint64_t(node_max));
        CHECK(dst_i >= 0 && uint64_t(dst_i) <= uint64_t(node_max));
        CHECK(len_i >= 0 && uint64_t(len_i) <= uint64_t(length_max));
        g.add_edge({node(src_i), node(dst_i), edge_len(len_i)});
    }
    return is;
}

std::vector<edge> digraph::to_edges() const {
    std::vector<edge> edg;
    for (node u : nodes()) {
        for (auto e : out_neighb[u]) { edg.push_back({ u, e }); }
    }
    return edg;
}

bool digraph::operator==(const digraph & o) {
    std::vector<edge> edg = to_edges();
    std::vector<edge> oth = o.to_edges();
    std::sort(edg.begin(), edg.end());
    std::sort(oth.begin(), oth.end());
    return edg == oth;
}

digraph digraph::reverse() const {
    digraph bwd;
    if (_n > 0) { bwd.add_node(node(_n-1u)); }
    for (node u : nodes()) {
        for (auto e : out_neighb[u]) { bwd.add({ e.dst, u, e.len }); }
    }
    return bwd;
}

digraph digraph::no_loop() const {
    digraph g;
    if (_n > 0) { g.add_node(node(_n-1u)); }
    for (node u : nodes()) {
        for (auto e : out_neighb[u]) {
            if (u != e.dst) { g.add_edge(u, e); }
        }
    }
    return g;
}

bool digraph::try_edge_update(node u, node v, edge_len l) {
    for (head & hd : out_neighb[u]) {
        if (hd.dst == v) {
            if (l < hd.len) { hd.len = l; }
            return true;
        }
    }
    return false;
}

std::pair<digraph, std::vector<node>>
digraph::subgraph(std::function<bool(node)> filter) {
    const node invalid = node(_n);
    std::vector<node> index_orig, index(_n, invalid);
    digraph h;
    auto add = [&index_orig, &index, invalid](node v) {
        if (index[v] == invalid) {
            index[v] = index_orig.size();
            index_orig.push_back(v);
        }
        return index[v];
    };
    for (node u : nodes()) {
        if (filter(u)) {
            for (auto hd : out_neighbors(u)) {
                if (filter(hd.dst)) {
                    h.add_edge(add(u), add(hd.dst), hd.len); 
                }
            }
        }
    }
    return std::make_pair(h, index_orig);
}


// ------------- unit test -----------

#include <algorithm>
#include <fstream>

namespace unit {

    digraph dg_small_ids;
    
    void test_digraph() {
        std::ifstream file_small("test_data/small.txt");
        file_small >> dg_small_ids;
        std::cout << dg_small_ids
                  <<"inp: n="<<  dg_small_ids.nb_nodes()
                  <<" m="<< dg_small_ids.nb_edges() <<"\n";
        std::vector<edge> edges = {
            {0, 1, 1}, {1, 2, 1}, {2, 3, 1}, {3, 4, 1},
            {0, 4, 10}, {2, 5, 10}, {5, 6, 1}, {6, 3, 1},
            {4, 0, 3}, {5, 1, 1}, {3, 3, 2},
            {4, 7, 10}, {7, 8, 5}, {8, 9, 2}, {9, 7, 1}, {6, 9, 1}, {10, 10, 1}
        };
        /* That is :

      _____10________
     /               \/
    0_                4 ---10---> 7_---5---> 8
    |\______3________/^            \         |
    |                 |             1        2
    1                 1              \       |
    |                 |               \      v
    v                 |                \----_9         10 <----
    1_--1--> 2 --1--> 3 <----               /            \    /
     \       |        ^\    /              /              \  1
      \      |        | \  2              /                \/
       1     10       |  \/              /
        \    |        1                 /
         \   v        |                /
          \- 5 --1--> 6 --------1-----/

        */
        
        for (auto e : edges) { std::cout << e <<"\n"; }
        digraph g;
        for (auto e : edges) { g.add(e); }
        CHECK(g == dg_small_ids);
        std::cout << g <<"\n";
        digraph h;
        std::cout << h <<"\n";
        h = g;
        CHECK(g == h);        
        std::vector<edge> hedg = h.to_edges();
        std::sort(edges.begin(), edges.end());
        std::sort(hedg.begin(), hedg.end());
        CHECK(edges == hedg);
    }
    
}
