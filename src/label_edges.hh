#pragma once

#include <unordered_map>
#include <vector>
#include <fstream>

#include "basics.hh"
#include "digraph.hh"

// Edges of a graphs with arbitrary labels that are mapped to indexes.
struct label_edges {

    std::vector<std::string> labels;
    std::unordered_map<std::string, node> indexes;
    std::vector<edge> edges;

    // Add a label if not present, and return its index.
    node add_label(const std::string & lab) ;

    // Get the index of label [lab]. It asserts the label is present.
    node index(const std::string & lab) {
        assert(indexes.count(lab) == 1);
        return indexes[lab];
    }

    bool has_index(const std::string & lab) {
        return indexes.count(lab) != 0;
    }

    std::string label(node i) {
        assert(i < labels.size());
        return labels[i];
    }

    /** Read edges from a file, or std::cin if fname is "-".
     *  Each line should be a triple [src dst edge_len].
     *  Lines beginning with '#' are ignored.
     */
    label_edges(std::string fname) ;

    label_edges() {}
    
    void parse_istream(std::istream & is) ;
};

namespace unit {
    
    extern digraph dg_small_labs, dg_road;
    extern label_edges edges_road;
    
    void test_label_edges();
}
