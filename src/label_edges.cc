#include <sstream>

#include "label_edges.hh"

namespace ch {

node label_edges::add_label(const std::string & lab) {
    if (indexes.count(lab) == 0) {
        node i = node(labels.size());
        labels.push_back(lab);
        indexes[lab] = i;
        return i;
    } else {
        return indexes[lab];
    }
}

label_edges::label_edges(std::string fname) {
    if (fname == "-") { parse_istream(std::cin); }
    else {
        std::ifstream file(fname);
        CHECK(file.is_open());
        parse_istream(file);
        file.close();
    }
}
    
void label_edges::parse_istream(std::istream & is) {
    std::string src_s, dst_s, len_s, line;
    const auto length_max = std::numeric_limits<edge_len>::max();
    
    while ( ! is.eof()) {
        is >> std::ws;
        if (is.eof()) return;
        std::getline(is, line);
        while (line.size() > 0 && line[0] == '#') {
            if (is.eof()) return;
            std::getline(is, line);
        }
        std::istringstream iss(line);
        CHECK( ! iss.eof());
        iss >> src_s;
        CHECK( ! iss.eof());
        iss >> dst_s;
        CHECK( ! iss.eof());
        iss >> len_s;
        CHECK(iss.eof());
        auto len_i = std::stoll(len_s);
        CHECK(len_i >= 0 && uint64_t(len_i) <= uint64_t(length_max));
        auto src_i = add_label(src_s);
        auto dst_i = add_label(dst_s);
        edges.push_back({src_i, dst_i, edge_len(len_i)});
    }
}


namespace unit {

    digraph dg_small_labs, dg_road;
    label_edges edges_road;
    
    void test_label_edges() {
        label_edges edg("test_data/small.txt");
        for (node i : irange<node>(node(0), node(7))) {
            edg.index(std::to_string(i));
        }
        CHECK(edg.indexes.count("not a label") == 0);
        
        CHECK(edg.labels.size() == 11);
        CHECK(edg.edges.size() == 17);
        for (auto e : edg.edges) { dg_small_labs.add(e); }
        std::cout << dg_small_labs <<"\n";
        CHECK(dg_small_labs == dg_small_ids); // ids appear in order the file

        edges_road = label_edges("test_data/road_corsica.txt");
        std::cout << edges_road.labels.size() <<" labels and "
                  << edges_road.edges.size() <<" edges.\n";
        digraph & g = dg_road;
        for (edge e : edges_road.edges) { g.add_edge(e); }
        std::cout << g.nb_nodes() <<" ndoes and "
                  << g.nb_edges() <<" edges.\n";
        CHECK(g.out_degree(edges_road.index("2272544925")) == 4);
        CHECK(g.out_degree(edges_road.index("59862146")) == 2);
    }

}

}

