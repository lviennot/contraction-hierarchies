#include "contraction.hh"
#include "label_edges.hh"
#include <ctime>

using namespace ch;

int main(int argc, char **argv) {

    assert(argc == 2);
    auto n_nodes = std::stoll(std::string(argv[1]));
    std::cout <<"Test from n_nodes="<< n_nodes <<"\n"<< std::flush;
    
    digraph g;
    label_edges edges_road = label_edges("test_data/road_corsica.txt");
    for (edge e : edges_road.edges) { g.add_edge(e); }
    std::cout <<"road graph : n="<< g.nb_nodes()
              <<" m="<< g.nb_edges() <<"\n";

    // All pairs with dijkstra:
    {
        auto start = std::chrono::high_resolution_clock::now();
        traversal trav;
        std::size_t n = std::min(std::size_t(n_nodes), g.nb_nodes());
        const std::size_t incr = g.nb_nodes() > n ? g.nb_nodes()/n : 1;
        for (std::size_t i = 0; i < g.nb_nodes() ; i += incr) {
            node u(i);
            trav.dijkstra(g, u);
        }
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast
            <std::chrono::milliseconds>(stop - start);
        std::cerr << n <<" x "<< n  <<" pairs: "<< duration.count() <<" ms\n";
    }


    // Distance oracle with contraction hierarchies.
    
    contraction contr(g);
    digraph g_ch = contr.contract();
    std::cout <<"contraction : n="<< g_ch.nb_nodes()
              <<" m="<< g_ch.nb_edges() <<"\n";
        
    // Check distances:
    auto start = std::chrono::high_resolution_clock::now();
    traversal trav;
    std::size_t n = std::min(std::size_t(n_nodes), g.nb_nodes());
    const std::size_t incr = g.nb_nodes() > n ? g.nb_nodes()/n : 1;
    for (std::size_t i = 0; i < g.nb_nodes() ; i += incr) {
        node u(i);
        for (std::size_t j = 0; j < g.nb_nodes() ; j += incr) {
            node v(j);
            contr.distance(u, v);
        }
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast
        <std::chrono::milliseconds>(stop - start);
    std::cerr << n <<" x "<< n  <<" CH queries: "<< duration.count() <<" ms\n";

}
        

