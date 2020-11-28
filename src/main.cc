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
    
    std::cerr <<"\nUsage: "<< argv[0] <<" [graph] [subset] [max_deg]\n"
              << paragraph (
        "\nContracts nodes of the graph in file [graph] until average degree "
        "reaches [max_deg]. Nodes from [subset] are never contracted. "
        "More precisely, in the Contraction Hierarchies style, when a node "
        "is contracted (removed from the graph), edges are added so that "
        "distances in the graph are preserved." )
              << paragraph (
        "\nInput format for [graph]: one edge per line with format: "
        "[src] [dst] [length]" )
              <<"Input format for [subset]: one node per line.\n";
        exit(1);
}


int main (int argc, char **argv) {

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
              <<" (overflow at "<< dist_max <<")\n";

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

