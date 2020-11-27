// Author: Laurent Viennot, Inria, 2020.

/** 
 * Various variations of a graph implementation based on vectors of vectors:
 * directed or undirected, weighted or unweighted.
 *
 * Example:
 *
 *
 * More detail:
 *
 * Nodes are indexes from 0 to n-1 (where is the number of nodes) stored
 * as uint32 or uint64.
 *
 * To use more general node labels, it is recommended to build a table of
 * all labels, and use its indexes as nodes.
 *
 * Graphs can grow : edge and node insertions are allowed (but not removal).
 * 
 * Graphs are multi-digraphs : 
 *  - several insertions of the same edge results in multiple copies of it,
 *  - undirected graphs are represented as symmetric digraphs,
 *  - a bidirectional version allows to access in-neighbors in addition to
 *    out-neighbors.
 *
 * The implementation should be rather fast for algorithms based on scanning
 * neighborhoods as this results in linear scans of a vectors.
 *
 * The size of a graph is linear (in number of nodes and edges).
 *
 *
 * Basic example:
 *
 *  #include "digraph.hh"
 *
 *    digraph<uint, uint> g;
 *
 *    g.add_edge(0, 1);
 *    g.add_edge(0, 2);
 *    g.add_edge(1, 2);
 *    
 *    // Acts as with a vector of lists of ints :
 *    for (uint u : g) {
 *        std::cout << u <<" -> ";
 *        for (uint v : g[u]) { std::cout << v <<" "; } 
 *        std::cout <<"\n";
 *    }
 *
 *    // or more informatively :
 *    for (uint u : g.nodes()) {
 *        std::cout << u <<" -> ";
 *        for (uint v : g.out_neighbors(u)) { std::cout << v <<" "; } 
 *        std::cout <<"\n";
 *    }
 *
 *
 * Notes:
 *  - A generic type I is used for index of nodes, allowing to use a dedicated
 *    type with restricted compability with usual integer types.
 *  - A generic type EH is used for representing an edge head, that is a
 *    destination of an arc which is possibly associated with a weight,
 *    or a pointer to more information on the edge. 
 *    Casting an edge head to I should yiel the index of the destination.
 *  - A bidirectional implementation allows to access both out-neighbors
 *    and in-neighbors. Using a different type ET for edge tails allows
 *    to reduce risks of confusion between head and tails.
 *
 *
 * More evolved bi-directional weighted-graph example:
 *
 *  #include "uint_index.hh"
 *  #include "saturated_int.hh"
 *  #include "edge.hh"
 *  #include "digraph.hh"
 *
 *  struct _node;
 *  using node = uint_index<uint_least32_t, _node>;
 *
 *  struct _weight;
 *  using weight = saturated_int<uint_least32_t, _weight>;
 *
 *  using head = edge::dst_wgt<node, weight>;
 *
 *  using graph = digraph<node, head>;
 *
 *    graph g;
 *    node v2(2);
 *    for (auto hd : g.out_neighbors(v2)) { 
 *        std::cout << (node)hd <<","<< hd.weight() <<" "; 
 *    }
 *
 *
 */

#pragma once

#include <stdint.h>
#include <cassert>

#include "uint_index.hh"
#include "saturated_int.hh"
#include "range.hh"
#include "edge.hh"
#include "vect_of_vect.hh"
#include "traversal.hh"




template <typename I = uint_least32_t, // int type for node index
          typename W = I,              // type for edge weight
          typename D = W>           // type for distances (sum of edge weights)
class make_graphs {

    struct _node; // dummy type for defining node type
    struct _weight; // dummy type for defining weight and distance types.

public:

    using node = uint_index <I, _node> ;

    using weight = saturated_int <W, _weight> ;
    using dist = saturated_int <D, _weight> ;

    using head = edge::dst_wgt <node, weight> ;
    using weighted_edge = edge::src_dst_wgt <node, weight> ;
    using edge = edge::src_dst <node> ;

    template <typename EH>
    struct digraph : public vect_of_vect <node, EH> {
        using graph = digraph<EH>;
        using node = node;
        using head = EH;
        using vvect = vect_of_vect<node, head>; // inherit constructors
        using vvect::vvect;
        void add_edge (node u, head hd) { vvect::add(u, hd); }
        std::size_t nb_nodes() const { return vvect::_n; }
        std::size_t nb_edges() const { return vvect::_m; }
        std::size_t out_degree(node u) const { return vvect::vect_size(u); }
        using vrange = typename vvect::vrange;
        vrange out_neighbors(node u) const { return vvect::operator[](u); }
    };

    // Undirected graphs are symmetric digraphs:
    template <typename EH>
    struct symdigraph : public digraph<EH> {
        using graph = symdigraph<EH>;
        using di_graph = digraph<EH>;
        void add_edge(EH tl, EH hd) {
            di_graph::add_edge(tl, hd);
            di_graph::add_edge(hd, tl);
        }
        std::size_t nb_edges() const {
            assert(di_graph::_m % 2 == 0);
            return di_graph::_m / 2;
        }
    };
    
    // Bidirectional digraph (access both out-edges and in-edges).
    template <typename EH>
    class bidigraph : public digraph<EH> {
    protected:
        using di_graph = digraph<EH>;
        di_graph bwd_graph;
    public:
        using graph = bidigraph<EH>;
        using vrange = typename di_graph::vrange;
        vrange in_neighbors(node u) const { return bwd_graph.out_neighbors(u); }
        std::size_t in_degree(node u) const { return bwd_graph.out_degree(u); }
        void add_node(node u) {
            di_graph::add_node(u);
            bwd_graph.add_node(u);
        }
        void add_edge(head tl, head hd) {
            di_graph::add_edge(tl, hd);
            bwd_graph.add_edge(hd, tl);
        }
        const di_graph & backward_graph() { return bwd_graph; }
    };

    // Unweighted classes of graphs :
    struct directed : public digraph <node> {
        using graph = directed;
        using edge = edge;
        void add(edge e) { digraph<node>::vvect::add(e.src, e); }
        void add_edge(edge e) { add(e); }
    };
    struct undirected : public symdigraph <node> {
        using graph = directed;
        using edge = edge;
        void add(edge e) { symdigraph<node>add_edge(e.src, e); }
        void add_edge(edge e) { add(e); }
    };
    struct bidirectional : public bidigraph <node> {
        using graph = directed;
        using edge = edge;
        void add(edge e) { bidigraph<node>add_edge(e.src, e); }
        void add_edge(edge e) { add(e); }
    };

    // unsigned-int-weighted gaphs:
    struct directed_weighted_uint : public digraph<head> {
        using graph = directed_weighted_uint;
        using weight = weight;
        using edge = weighted_edge;
        void add_edge(node u, node v, weight w) {
            digraph<head>::add_edge(u, head(v, w));
        }
    };
    struct undirected_weighted_uint : public symdigraph<head> {
        using graph = undirected_weighted_uint;
        using weight = weight;
        using edge = weighted_edge;
        void add_edge(node u, node v, weight w) {
            symdigraph<head>::add_edge(head(u,w), head(v, w));
        }
    };
    struct bidirectional_weighted_uint : public bidigraph<head> {
        using graph = undirected_weighted_uint;
        using weight = weight;
        using edge = weighted_edge;
        void add_edge(node u, node v, weight w) {
            bidigraph<head>::add_edge(u, head(v, w));
        }
    };
    
    using trav = traversal <digraph<head>, dist> ;
};

using graph = make_graphs<unsigned int>;
using graph32 = make_graphs<uint_least32_t, uint_least32_t, uint64_t>;
using graph64 = make_graphs<uint64_t>;

namespace unit {
    void test_graph();
}
