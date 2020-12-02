// Author: Laurent Viennot, Inria, 2020.

#include "traversal.hh"
#include "label_edges.hh"

namespace ch {

namespace unit {

    void test_traversal() {

        traversal<digraph> trav, bwd_trav;

        // test small graph

        digraph fwd = dg_small_ids;
        digraph bwd = fwd.reverse();
        
        for (node u : fwd) {
            trav.dijkstra(fwd, u);
            std::vector<dist> u_dist = trav.copy_distances();
            for (node v : fwd) {
                dist d = trav.bidir_dijkstra(fwd, bwd, bwd_trav, u, v);
                CHECK(d == u_dist[v]);
            }
        }

        // test road graph
            
        fwd = dg_road;
        bwd = fwd.reverse();

        std::cout <<"trav: graph of "<< fwd.nb_nodes() <<" nodes "
                  <<" and "<< fwd.nb_edges() <<" edges\n";
            
        std::vector<std::string>
            labs = { "50532632", "82568690", "339428091", "245917016",
                     "2502605852", "340493863", "343442277", "404670649"};
        std::vector<node> ids;
        for (auto s : labs) { ids.push_back(edges_road.index(s)); }

        for (node u : ids) {
            trav.dijkstra(fwd, u);
            std::vector<dist> u_dist = trav.copy_distances();
            for (node v : ids) {
                dist d = trav.bidir_dijkstra(fwd, bwd, bwd_trav, u, v);
                CHECK(d == u_dist[v]);
                std::cout << d <<" ";
            }
            std::cout <<"\n";
        }
         
    }
}

}
