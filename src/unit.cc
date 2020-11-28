#include <iostream>

#include "digraph.hh"
#include "label_edges.hh"
#include "traversal.hh"
#include "contraction.hh"

using namespace ch;

int main() {

    node u(1), v(2); edge_len l(3);
    edge_head hd(v, l); edge e(u, v, l);
    CHECK(node(hd) == v && hd.head() == v && hd.length() == l);
    CHECK(node(e) == v && e.head() == v && e.length() == l && e.tail() == u);

    std::cerr <<" ----------- test_digraph()\n" << std::flush;
    unit::test_digraph();
    std::cerr <<" ----------- test_label_edges()\n" << std::flush;
    unit::test_label_edges();
    std::cerr <<" ----------- test_traversal()\n" << std::flush;
    unit::test_traversal();
    std::cerr <<" ----------- test_contraction()\n" << std::flush;
    unit::test_contraction();
    
    std::cerr <<"Unit tests done.\n";
    assert(false); // To check if assert() is active or not.
}
