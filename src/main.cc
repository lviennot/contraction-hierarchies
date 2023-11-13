#include <fstream>
#include <sstream>
#include <vector>

#include "basics.hh"
#include "label_edges.hh"
#include "digraph.hh"
#include "contraction.hh"

using namespace ch;

void usage_exit (char **argv) {
    auto paragraph = [](std::string s, int width=80) -> std::string {
        std::string acc;
        while (s.size() > 0) {
            int pos = s.size();
            if (pos > width) pos = s.rfind(' ', width);
            std::string line = s.substr(0, pos);
            acc += line + "\n";
            s = s.substr(pos);
        }
        return acc;
    };
    
    std::cerr <<"\nUsage: "<< argv[0] <<" [-hierarchies] [graph] [subset] [max_deg]\n"
              << paragraph (
        "\nContracts nodes of the graph in file [graph] until average degree "
        "reaches [max_deg]. Nodes from [subset] are never contracted. "
        "More precisely, in the Contraction Hierarchies style, when a node "
        "is contracted (removed from the graph), edges are added so that "
        "distances in the graph are preserved." )
              << paragraph (
        "\nInput format for [graph]: one edge per line with format: "
        "[src] [dst] [length]" )
              <<"Input format for [subset]: one node per line."
              << paragraph(
                           "\nOutputs a distance preserver for nodes in [subset] (i.e. a graph with node set containing [subset] with same distances as in the original graph, and with average degree at most [max_deg]). If option [-hierarchies] is given then it instead outputs the contraction hierarchies (i.e. a graph with same node set and same distances where any pair of nodes are linked by a few hops shortest path), the contraction order is given as a comment line."
                           )
        ;
        exit(1);
}


int main (int argc, char **argv) {

    // ------- helper functions for manipulating args ----------
    auto i_arg = [&argc,&argv](std::string a) {
        for (int i = 1; i < argc; ++i)
            if (a == argv[i])
                return i;
        return -1;
    };
    auto del_arg = [&argc,&argv,i_arg](std::string a) {
        int i = i_arg(a);
        if (i >= 0) {
            for (int j = i+1; j < argc; ++j)
                argv[j-1] = argv[j];
            --argc;
            return true;
        }
        return false;
    };

    bool do_graph = del_arg("-graph");
    bool do_hierarchies = del_arg("-hierarchies");
    
    // ------------------------ usage -------------------------
    if (argc != 4) {
        usage_exit(argv);
    }
    std::string fgraph (argv[1]);
    std::string fsubset (argv[2]);
    float max_deg = std::stof(argv[3]);

    // ------------------------- load graph ----------------------
    label_edges labedg(fgraph);
    digraph g;
    for (auto e : labedg.edges) { g.add(e); }
    std::cerr <<"loaded graph with n=" << g.n() << " nodes"
              <<" and m=" << g.m() <<" edges\n";
    dist maxlen = 0;
    for (auto e : labedg.edges) {
        if (e.len > maxlen) { maxlen = e.len; }
    }
    std::cerr <<"maximum edge length: "<< maxlen
              <<" (distance overflow at "<< dist_max <<")\n";

    if (do_graph) {
        std::cout << g;
    }

    // ------------------------- load subset -----------------------
    std::vector<node> subset;
    std::ifstream input(fsubset);
    CHECK(input.is_open());
    for (std::string line; std::getline(input, line); ) {
        subset.push_back(labedg.index(line));
    }
    input.close();
    std::cerr << "loaded subset of "<< subset.size() <<" nodes\n";
    
    // ------------------------- contraction -----------------------
    contraction  ch(g, subset);
    digraph g_ch = ch.contract(max_deg);
    std::cerr << "contraction\n";

    // ----------------------------- output ------------------------
    if (do_hierarchies) {
        std::vector<node> contr_order(ch.contraction_order());
        std::cout <<"# contraction_order:";
        for (node u : contr_order) { std::cout <<" "<< u; }
        std::cout <<"\n";
        for (node u : g_ch) {
            for (auto e : g_ch[u]) {
                std::cout << u <<"\t"<< e.dst <<"\t"<< e.len <<"\n";
            }
        }
    } else {
        auto subind = g_ch.subgraph
            ([&ch](node v){ return ch.in_contracted_graph(v); });
        for (node u : subind.first) {
            for (auto e : subind.first[u]) {
                std::cout << labedg.label(subind.second[u])
                          <<"\t"<< labedg.label(subind.second[e.dst])
                          <<"\t"<< e.len <<"\n";
            }
        }
    }
}

