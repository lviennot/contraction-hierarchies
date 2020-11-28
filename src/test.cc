#include <inttypes.h>
#include <iostream>

#include "uint_index.hh"
#include "saturated_int.hh"
#include "range.hh"
#include "edge.hh"
#include "digraph.hh"
#include "traversal.hh"

struct _A;
using ida = uint_index<uint_least32_t, _A>;

struct _B;
using idb = uint_index<uint_least32_t, _B>;


struct _node;
using node = uint_index<uint_least32_t, _node>;

struct _weight;
using weight = saturated_int<uint_least32_t, _weight>;
using weight64 = saturated_int<uint64_t, _weight>;
    
using head = edge::dst_wgt<node, weight>;
using graph = bidigraph<node, head>;


int main() {
    {
        weight w = 32;
        weight64 x(64);
        std::cout << x + weight64(w) <<"\n";
    }
    
    ida a = ida(1);
    std::cout << std::hash<ida>()(a) <<"\n";
    idb b = idb(2);
    size_t i = a;
    //a = b; // fails to compile
    //ida a2 = ++a; // fails to compile
    const ida a2 = a;
    std::cout <<"\na="<< a <<" a2="<< a2 <<" b="<< b <<" i="<< i <<"\n";

    std::vector<int> t = { 1,2,3,4 };
    for (ida a : utl::irange_rev<ida>(ida(0), ida(4))) std::cout << t[a] <<" ";
    std::cout <<"\n";

    weight c = 12;
    std::cout << c+30 <<" "<< std::numeric_limits<weight>::max() <<"\n";
    //satb d = 30;
    //std::cout << c+d <<"\n";
    //int j = 30; c+j;
    std::cout << c + int(3) <<"\n";
    weight e = weight::infinity;
    std::cout <<"e="<< e <<" c+e="<< c+e <<"\n";

    
    std::cout <<"\n--------------------------------------------------\n";

    {
    digraph<uint, uint> g;

    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(1, 2);
    
    for (uint u : g) {
        std::cout << u <<" -> ";
        for (uint v : g[u]) { std::cout << v <<" "; } 
        std::cout <<"\n";
    }

    std::cout <<"\n---------------\n";

    for (uint u : g.nodes()) {
        std::cout << u <<" -> ";
        for (uint v : g.out_neighbors(u)) { std::cout << v <<" "; } 
        std::cout <<"\n";
    }

    std::cout <<"\n---------------\n";
    }
    
    graph g;
    node u0(0), u1(1), u2(2), u3(3), u4(4);

    auto edge = [&g](node u, weight len, node v) {
        g.add_edge(head(u,len), head(v,len));
    };
    edge(u0, 1, u1);
    edge(u0, 10, u2);
    edge(u1, 1, u2);
    edge(u1, 1, u3);
    edge(u3, 1, u4);
    edge(u2, 1, u4);
    edge(u3, 1, u0);
    edge(u2, 1, u1);


    for (node u : g) {
        std::cout << u <<" -> ";
        for (head hd : g[u]) {
            std::cout << hd.dst <<","<< hd.wgt <<" ";
        } 
        std::cout <<"\n";
    }

    std::cout <<"\n---------------\n";

    for (node u : g.nodes()) {
        std::cout << u <<" : ";
        for (auto e : g.in_neighbors(u)) {
            std::cout << node(e) <<","<< e.weight() <<" ";
        }
        std::cout <<"\n";
    }

    std::cout <<"\n"<< g.nb_nodes() <<" nodes and "
              << g.nb_edges() <<" edges.\n";

    traversal<digraph<node, head>, uint64_t> trav;
    trav.dijkstra(g.reverse(), u3);
    for (node u : g.nodes()) {
        std::cout <<"dist "<< u <<" : "<< trav.distance(u) <<"\n";
    }
    std::cout <<"\n---------------\n";
    trav.dijkstra(g, u0);
    for (node u : g.nodes()) {
        std::cout <<"dist "<< u <<" : "<< trav.distance(u) <<"\n";
    }
    std::cout <<"\n---------------\n";
    digraph<node, head> h(g);
    trav.dijkstra(h, u0);
    for (node u : h.nodes()) {
        std::cout <<"dist "<< u <<" : "<< trav.distance(u) <<"\n";
    }


}
