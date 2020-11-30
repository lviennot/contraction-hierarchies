// Author: Laurent Viennot, Inria, 2020.

/** Growable multi-digraph implementation when vertices are indexed 
 * from 0 to n-1.
 * The graph can only grow: add an edge from a node to another, or add 
 * vertex n,n+1,...,n'. 
 * n and m designate the number of nodes and edges respectively.
 *
 * Basic example:
 *
 *  #include "digraph.hh"
 *
 *    digraph g;
 *
 *    g.add_edge(0, 1, 12); // edge from 0 to 1 with length 12
 *    g.add_edge(0, 2, 13);
 *    g.add_edge(1, 2, 14);
 *    
 *    // It is basically a vector of vectors :
 *    for (node u : g) {
 *        std::cout << u <<" -> ";
 *        for (auto e : g[u]) { std::cout << e.dst <<" "<< e.len <<" "; } 
 *        std::cout <<"\n";
 *    }
 *
 */

#pragma once

#include <vector>
#include <functional>

#include "basics.hh"
#include "ranges.hh"

// forward declare friend operators
namespace ch { class digraph; }
std::ostream& operator<<(std::ostream & os, const ch::digraph & g) ;
std::istream& operator>>(std::ostream & is, ch::digraph & g) ;


namespace ch {

// A digraph as a vector of vectors.
class digraph {

public:

    using node = node;
    using head = edge_head;
    using edge = edge;
    using graph = digraph;

protected:
    
    std::vector<std::vector<head>> out_neighb;
    std::size_t _n; // number of nodes
    std::size_t _m; // number of edges

public:

    digraph() : _n(0), _m(0) {}

    std::size_t nb_nodes() const { return _n; }
    std::size_t n() const { return _n; } // almost standard

    std::size_t nb_edges() const { return _m; }
    std::size_t m() const { return _m; } // almost standard

    std::size_t out_degree(node u) const { return out_neighb[u].size(); }
    
    void add_node(node u) {
        if (u >= _n) {
            _n = std::size_t(u) + 1;
            out_neighb.resize(_n);
        }
    }

    // We don't check if the edge is already there (multi-digraph).
    void add_edge(node src, head hd) {
        const node dst = hd;
        add_node(src);
        add_node(dst);
        out_neighb[src].push_back(hd);
        ++_m;
    }
    
    void add_edge(node src, node dst, edge_len l) { add_edge(src, head(dst,l));}
    void add_edge(edge e) { add_edge(e.src, e); }
    void add(edge e) { add_edge(e.src, e); }

    // If edge src->dst is present and has length greater than [l],
    // update its length to [l]. If the edge is not present, add it with 
    // length [l].
    // Returns true if the edge was added;
    bool update_edge(node src, node dst, edge_len l) ;
    
    irange<node> nodes() const { return irange<node>(node(0), node(_n)); }
    
    // iterator for the graph itself is equivalent to nodes()
    int_iterator<node> begin() const { return int_iterator<node>(node(0));}
    int_iterator<node> end() const { return int_iterator<node>(node(_n)); }

    
    using hrange = crange<typename std::vector<head>>;

    hrange out_neighbors(node u) const ;

    // an alias for out_neighbors() :
    hrange operator[](node u) const { return out_neighbors(u); } 

    friend std::ostream& ::operator<<(std::ostream & os, const digraph & g) ;
    friend std::istream& ::operator>>(std::ostream & is, digraph & g) ;

    // simple manipulations:
    std::vector<edge> to_edges() const ;
    bool operator==(const digraph & o) ;
    digraph reverse() const ;
    digraph no_loop() const ;

    // Compute  a subgraph (nodes are re-indexed) :
    std::pair<digraph, std::vector<node>>
        subgraph(std::function<bool(node)> filter) ;
};


namespace unit {

    extern digraph dg_small_ids;
    
    void test_digraph() ;

}

}
