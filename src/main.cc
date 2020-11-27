#include <fstream>
#include <sstream>
#include <vector>

#include "basics.hh"
#include "label_edges.hh"
#include "digraph.hh"
#include "contraction.hh"


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
    std::vector<node> sel;
    std::ifstream input(fsubset);
    CHECK(input.is_open());
    for (std::string line; std::getline(input, line); ) {
        sel.push_back(labedg.index(line));
    }
    input.close();
    std::cerr << "loaded subset of "<< sel.size() <<" nodes\n";
    std::set<node> contractible;
    for (node u : g) contractible.insert(u);
    for (node u : sel) contractible.erase(u);
    std::cerr << "contractibles : "<< contractible.size() <<" nodes\n";
    
    // ------------------------- contraction -----------------------
    contraction  ch(g, contractible);
    digraph g_ch = ch.contract(max_deg);
    std::cerr << "contraction\n";

    // ----------------------------- output ------------------------
    for (node u : g_ch) {
        if (ch.in_contracted_graph(u)) {
            for (auto e : g_ch[u]) {
                if (ch.in_contracted_graph(e.dst)) {
                    std::cout << labedg.label(u)
                              <<"\t"<< labedg.label(e.dst)
                              <<"\t"<< e.len <<"\n";
                }
            }
        }
    }
}

