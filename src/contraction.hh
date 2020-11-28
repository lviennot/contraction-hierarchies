// Author: Laurent Viennot, Inria, 2020.

#pragma once

#include <vector>
#include <queue>
#include <set>
#include <algorithm>

#include "basics.hh"
#include "digraph.hh"
#include "traversal.hh"

namespace ch {

class contraction {

protected:
    digraph fwd, bwd;
    traversal<digraph> trav_fwd, trav_bwd;
    std::set<node> contractible;
    std::vector<node> contract_order;
    std::vector<bool> in_contracted_gr;
    std::vector<std::size_t> contract_rank;
    std::size_t current_rank;
    std::size_t n, m; // number of node and edges in current contracted graph
    std::vector<node> in_degrees, out_degrees;

public:

    // Prepare for contracting [g]. Nodes in [keep] will not be contracted.
    contraction(const digraph &g, const std::vector<node> &keep = {}) ;

    // Contract nodes successively while average degree is bellow [max_avg_deg].
    digraph & contract(float max_avg_deg
                       = std::numeric_limits<float>::max()) ;

    // Returns [true] if node [u] has not been contracted yet.
    bool in_contracted_graph(node u) const ;

    // Returns the order in which the nodes have been contracted.
    const std::vector<node> & contraction_order () const ;

    // Returns the distance between two nodes. Efficient after most nodes have
    // been contracted. (Not tested without full contraction.)
    dist distance(node src, node dst) ;

protected:

    struct vtx_deg {
        node vtx;
        std::size_t deg;
        vtx_deg(node vtx, std::size_t deg) : vtx(vtx), deg(deg) {}
    };

    static bool cmp_vtx_deg(vtx_deg left, vtx_deg right) ;

    // Estimation of the number of edges produced by contracting [u].
    std::size_t fill_degree(node u) ;
    static constexpr std::size_t max_shift8 = 0xff;


    // Returns number of nodes contracted.
    std::size_t contract_round() ;

    void contract_node(node u) ;

    // Try to update an edge is present. Return true if not.
    bool cannot_update_edge(node u, node v, dist l) ;    
};

namespace unit {
    void test_contraction();
}

}
