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
    
    std::cerr <<"\nUsage: "<< argv[0] <<" [graph]\n"
              << paragraph ("\nRead graph in file [graph].")
              << paragraph (
        "\nInput format for [graph]: one edge per line with format: "
        "[src] [dst] [length]" )
              <<"\n";
        exit(1);
}


int main (int argc, char **argv) {

    // ------------------------ usage -------------------------
    if (argc != 2) {
        usage_exit(argv);
    }
    std::string fgraph (argv[1]);

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
    bool sym = g.reverse() == g;
    std::cerr <<"graph is "<< (sym ? "" : "not ") << "symmetric\n";
}

