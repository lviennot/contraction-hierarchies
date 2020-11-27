#include <iostream>

#include "graph.hh"

namespace unit {

    void test_graph () {
        // test node types:
        graph::node::unit_test();
        graph32::node::unit_test();
        graph64::node::unit_test();

        graph::node v0(0), v1(1), v2(2), v3(3), v4(4), v5(5), v6(6);
        std::vector<graph::weighted_edge> edges = {
            {v0, v1, 1}, {v1, v2, 1}, {v2, v3, 1}, {v3, v4, 1},
            {v0, v4, 10}, {v2, v5, 10}, {v5, v6, 1}, {v6, v3, 1}
        };
        
        {
            using grp = graph::directed;
            grp::graph g;
            for (auto e : edges) { g.add(e); }
            for (grp::node u : g) {
                std::cout << u <<" -> ";
                for (grp::head hd : g[u]) {
                    std::cout << hd <<" ";
                } 
                std::cout <<"\n";
            }
            std::cout << g.nb_nodes() <<" nodes and "
                      << g.nb_edges() <<" edges\n";
        }

        {
            using grp = graph::undirected_weighted_uint;
            grp::graph g;
            for (auto e : edges) { g.add_edge(e.src, e.dst, e.wgt); }
            for (grp::node u : g) {
                std::cout << u <<" -> ";
                for (grp::head hd : g[u]) {
                    std::cout << hd.dst <<","<< hd.wgt <<" ";
                } 
                std::cout <<"\n";
            }
            std::cout << g.nb_nodes() <<" nodes and "
                      << g.nb_edges() <<" edges\n";
        }

        {
            using grp = graph::directed_weighted_uint;

            grp::graph g;
            grp::node u0(0), u1(1), u2(2), u3(3), u4(4);

            auto edge = [&g](grp::node u, grp::weight len, grp::node v) {
                g.add_edge(u, v, len);
            };
            edge(u0, 1, u1);
            edge(u0, 10,u2);
            edge(u1, 1, u2);
            edge(u1, 1, u3);
            edge(u3, 1, u4);
            edge(u2, 2, u4);
            edge(u3, 1, u0);
            edge(u2, 1, u1);

            for (grp::node u : g) {
                std::cout << u <<" -> ";
                for (grp::head hd : g[u]) {
                    std::cout << hd.dst <<","<< hd.wgt <<" ";
                } 
                std::cout <<"\n";
            }
            std::cout << g.nb_nodes() <<" nodes and "
                      << g.nb_edges() <<" edges\n"
                      <<"---------------\n";
        }
        
    }
    
}
