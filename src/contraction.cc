// Author: Laurent Viennot, Inria, 2020.

#include <iomanip>
#include <ctime>
#include <chrono>

#include "contraction.hh"
#include "label_edges.hh"

namespace ch {

contraction::contraction(const digraph &g, const std::vector<node> &keep)
    : fwd(g.no_loop()), bwd(fwd.reverse()),
      contractible(), in_contracted_gr(g.nb_nodes(), true),
      contract_rank(g.nb_nodes(), g.nb_nodes()), current_rank(0),
      in_degrees(g.nb_nodes()), out_degrees(g.nb_nodes())
{

    // statistices on subgraph induced by [in_contracted_gr]
    n = fwd.nb_nodes();
    m = fwd.nb_edges();
    for (node u : fwd) { out_degrees[u] = fwd.out_degree(u); }
    for (node u : bwd) { in_degrees[u] = bwd.out_degree(u); }
        
    // Contractible is the complement of keep:
    for (node u : g) contractible.insert(u);
    for (node u : keep) contractible.erase(u);
}
    
digraph & contraction::contract(float max_avg_deg) {
    std::size_t round = 0, last_round = 0;
    auto start = std::chrono::high_resolution_clock::now();
    while (true) {
        if (m >= max_avg_deg * n || contractible.empty()) break;
        std::size_t ncontracted = contract_round();
        ++round;
        if (round >= 3 * last_round / 2) {
            last_round = round;
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast
                <std::chrono::milliseconds>(now - start);
            std::cerr << "rnd="<< round
                      << std::fixed << std::setprecision(1)
                      <<" "<< duration.count() / 1000. <<"s"
                      <<" n="<< n <<" m="<< m
                      <<" nc="<< ncontracted
                      <<" avg_out_deg="<< (n == 0 ? 0 : float(m)/n)
                      <<" CH: m="<< fwd.nb_edges() <<"\n";
        }
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast
        <std::chrono::milliseconds>(stop - start);
    std::cerr <<"contracted graph: n="<< n <<" m="<< m
              << std::fixed << std::setprecision(1)
              <<" avg_out_deg="<< (n == 0 ? 0 : float(m)/n)
              <<" in "<< duration.count() / 1000. <<"s\n"
              <<"contraction hierarchies (CH) n="<< fwd.nb_nodes()
              <<" m="<< fwd.nb_edges() <<"\n" << std::flush;
    //for (auto i : contract_rank) { std::cerr <<" "<< i; } std::cerr<<"\n";
    return fwd;
}

bool contraction::in_contracted_graph(node u) const { return in_contracted_gr[u]; }
    
const std::vector<node> & contraction::contraction_order () const {
    return contract_order;
}

dist contraction::distance(node src, node dst) {
    return trav_fwd.bidir_dijkstra
        (fwd, bwd, trav_bwd,
         src, dst, trav_fwd.dist_infinity, true,
         [this](node v, dist d, node par) {
            return contract_rank[par] < contract_rank[v];
        });
}



bool contraction::cmp_vtx_deg(vtx_deg left, vtx_deg right) {
    return left.deg < right.deg;
}


constexpr std::size_t max_shift8 = 0xff;

std::size_t contraction::fill_degree(node u) {
    std::size_t dmin = in_degrees[u];
    std::size_t dmax = out_degrees[u];
    if (dmin > dmax) { std::swap(dmin, dmax); }
    assert(dmin <= dmax);
    if (dmin == 0) { return 0; } // edge removal only
    if (dmin == 1) { // Remove dmax + 1 edges and add dmax
        return std::min(dmax, max_shift8); // prefer small dmax
    }
    std::size_t fill = dmin * dmax - dmin - dmax;//>=dmax-dmin >= 0 as dmin-1>=1
    return (fill + 1) << 8;
}


// Returns number of nodes contracted.
std::size_t contraction::contract_round() {
    std::vector<vtx_deg> vtx;
    for (node u : contractible) { vtx.push_back({ u, fill_degree(u) }); }
    std::sort(vtx.begin(), vtx.end(), cmp_vtx_deg);

    std::vector<node> contr;
    const float min_pct = 1.0; // set stoping threshold at this percentage
    std::size_t fill_deg_thr = 0, i = 0, n = vtx.size();
    std::set<node> neighb_of_contr;

    for (auto ud : vtx) {
        if (i * 100 < min_pct * n) { fill_deg_thr = ud.deg; }
        else { if (4 * ud.deg > 5 * fill_deg_thr) { break; } }
        node u = ud.vtx;
        if ( neighb_of_contr.find(u) == neighb_of_contr.end()) {
            ++i;
            for (auto e : bwd.out_neighbors(u)) {
                neighb_of_contr.insert(e.dst);
            } 
            for (auto e : fwd.out_neighbors(u)) {
                neighb_of_contr.insert(e.dst);
            }
            contr.push_back(u);
        }
    }
    for (node u : contr) { contract_node(u); }
    return contr.size();
}

void contraction::contract_node(node u) {
    in_contracted_gr[u] = false;
    contract_rank[u] = current_rank++;
    contract_order.push_back(u);
    contractible.erase(u);
    --n;
    m -= in_degrees[u];
    m -= out_degrees[u];
    bool first_iter = true;
    for (auto e : bwd.out_neighbors(u)) {
        if (in_contracted_gr[e.dst]) {
            --(out_degrees[e.dst]); // u lost
            for (auto f : fwd.out_neighbors(u)) {
                if (in_contracted_gr[f.dst]) {
                    if (first_iter) { --(in_degrees[f.dst]); } // u lost
                    const dist d_ef = e.len + f.len;
                    if (e.dst != f.dst
                        && d_ef < trav_fwd.bidir_dijkstra
                        (fwd, bwd, trav_bwd,
                         e.dst, f.dst, d_ef, false,
                         [this](node x, dist d, node _) {
                            return in_contracted_gr[x];
                        })
                        ) {
                        const bool fadd = fwd.update_edge(e.dst, f.dst, d_ef);
                        const bool badd = bwd.update_edge(f.dst, e.dst, d_ef);
                        assert(fadd == badd);
                        if (fadd || badd) {
                            ++m;
                            ++(out_degrees[e.dst]);
                            ++(in_degrees[f.dst]);
                        }
                    }
                }
            }
            first_iter = false;
        }
    }
    if (first_iter) {
        for (auto f : fwd.out_neighbors(u)) {
            if (in_contracted_gr[f.dst]) {
                if (first_iter) { --(in_degrees[f.dst]); } // u lost
            }
        }
    }
}




namespace unit {

    void test_contraction() {

        for (digraph g : {dg_small_ids, dg_road}) {

        contraction contr(g);

        digraph g_ch = contr.contract(3);
        std::cout <<"contraction : n="<< g_ch.n() <<" m="<< g_ch.m() <<"\n";
        
        // Check distances:
        traversal<digraph> trav;
        std::size_t n = std::min(std::size_t(10), g.n()), i = 0;
        for (node u : g) {
            if (i < n && contr.in_contracted_graph(u)) {
                ++i;
                trav.dijkstra(g_ch, u,
                              [ & contr](node v, dist d) {
                                  return contr.in_contracted_graph(v);
                              });
                std::vector<dist> dist = trav.copy_distances();
                trav.dijkstra(g, u);
                for (node v : g) {
                    if (contr.in_contracted_graph(v)) {
                        CHECK(trav.distance(v) == dist[v]);
                    }
                }
            }
        }

        // Finish contraction:
        g_ch = contr.contract();
        std::cout <<"contraction : n="<< g_ch.n() <<" m="<< g_ch.m() <<"\n";
        
        std::vector<node> contr_order(contr.contraction_order());
        std::cout <<"contr_order:";
        i = 0; 
        for (node u : contr_order) { if (i++ < n) std::cout <<" "<< u; }
        std::cout <<"...\n";

        // Check CH dist:
        const std::size_t incr = g.n() > n ? g.n()/n : 1;
        for (std::size_t i = 0; i < g.n() ; i += incr) {
            node u(i);
            trav.dijkstra(g, u);
            for (std::size_t j = 0; j < g.n() ; j += incr) {
                node v(j);
                dist duv = contr.distance(u, v);
                //std::cout << duv <<" ";
                if (trav.distance(v) != duv) {
                    std::cerr <<"dist("<< u <<", "<< v <<") = "
                              << trav.distance(v) <<" vs "<< duv <<"\n"; 
                }
                CHECK(trav.distance(v) == duv);
            }
            //std::cout <<"\n";
        }

        }
        
    }
}

}
