// Author: Laurent Viennot, Inria, 2020.

// Necessary data structures for performing a Dijkstra like graph traversal.

#pragma once

#include <queue>
#include <vector>

#include "basics.hh"
#include "digraph.hh"

template <typename G = digraph> // graph type
class traversal {

public:
    using trav = traversal<G>;
    using graph = G;
    using node = node;
    using dist = dist;

protected:

    // For the priority queue:
    struct node_dist {
        node _node;
        dist _dist;
        node_dist(node _node, dist _dist) : _node(_node), _dist(_dist) {}
        node_dist() : _node(-1), _dist(dist_max) {}
        operator node() const { return _node; }
    };
    static bool node_dist_greater(node_dist a, node_dist b) {
        return b._dist < a._dist; // priority_queue::top() returns max element
    }
    
    std::vector<dist> distances;
    using queue_t = std::priority_queue <node_dist,
                                         std::vector<node_dist>,
                                   std::function<bool(node_dist, node_dist)>>;
    queue_t queue;
    std::vector<bool> visited;
    std::vector<node> visited_nodes;
    std::size_t capacity;

public:
    
    traversal() : queue(node_dist_greater), capacity(0) {}

    dist distance(node u) const { return distances[u]; }

    std::vector<dist> copy_distances() const {
        return std::vector<dist>(distances.begin(), distances.begin()+capacity);
    }
    
    void init(std::size_t n) {
        // TODO: find the best thresholds for prefering linear fill
        // to sparse reinitialization.
        
        // Queue has penalty in scanning elements but some cache locality:
        const std::size_t n_last = visited_nodes.size() + 2 * queue.size();
        if (n_last > capacity / 10) {
            std::fill(distances.begin(), distances.end(), dist_max);
            std::fill(visited.begin(), visited.end(), false);
            queue = queue_t(node_dist_greater);
        } else {
            for(node u : visited_nodes) {
                distances[u] = dist_max;
                visited[u] = false;
            }
            for ( ; ! queue.empty() ; queue.pop()) {
                const node u = queue.top();
                distances[u] = dist_max;
                visited[u] = false;
            }
        }
        visited_nodes.clear();

        if (n > distances.size()) {
            distances.resize(n, dist_max);
            visited.resize(n, false);
        }
        capacity = n;
    }

    void dijkstra(const graph & g, const node src,
                  std::function<bool(node, dist)> filter
                                     = [](node v, dist d) { return true; }
                  ) {
        init(g.nb_nodes());
        distances[src] = 0;
        queue.push(node_dist(src, 0));

        while ( ! queue.empty()) {
            node_dist ud = queue.top();
            queue.pop();
            node u = ud._node;
            if ( ! visited[u] ) {
                dist du = ud._dist;
                assert(du == distances[u]);
                visited[u] = true;
                visited_nodes.push_back(u);
                for (auto e : g.out_neighbors(u)) {
                    node v = e.dst;
                    dist dv = du + dist(e.len);
                    if (filter(v, dv) && dv < distances[v]) {
                        distances[v] = dv;
                        queue.push(node_dist(v, dv));
                    }
                }
            }
        }
    }

    // Returns the distance from [src] to [dst], assuming that [bwd] is the
    // reverse graph of [fwd].
    // The search is limited assuming [dist(src,dst) < dist_limit].
    // If its not the case, the value returned is at least [dist_limit].
    // Pruned search:
    // Only nodes [v] for which [filter(v, dv, par)] return [true] are visited.
    // If that prevents from visiting all nodes at distance less than [r]
    // before a node at distance [r], [pruned] must be set to [true].
    dist bidir_dijkstra(const graph & fwd, const graph & bwd, trav & bwd_trav, 
                        const node src, const node dst,
                        const dist dist_limit = dist_max, // for dist(src,dst)
                        const bool pruned = false, // search is pruned by:
                        std::function<bool(node, dist, node)> filter
                               = [](node v, dist d, node par) { return true; }
                  ) {
        // few sanity checks:
        assert(this != & bwd_trav);
        assert(fwd.nb_nodes() == bwd.nb_nodes()
                && fwd.nb_edges() == bwd.nb_edges());
        
        init(fwd.nb_nodes());
        bwd_trav.init(fwd.nb_nodes());

        distances[src] = 0;
        queue.push(node_dist(src, 0));
        bwd_trav.distances[dst] = 0;
        bwd_trav.queue.push(node_dist(dst, 0));
        dist cur_dist_src_dst = dist_max, fwd_radius = 0, bwd_radius = 0;

        while ( ! (queue.empty() && bwd_trav.queue.empty()) ) {
            fwd_radius = bidir_dijkstra_step
                (fwd, cur_dist_src_dst, dist_limit,
                 bwd_trav, dst, pruned ? 0 : bwd_radius, filter);
            if (fwd_radius == dist_max && ! pruned) { break; } //fwd search done
            bwd_radius =
                bwd_trav.bidir_dijkstra_step
                (bwd, cur_dist_src_dst, dist_limit,
                 (*this), src, pruned ? 0 : fwd_radius, filter);
            if (bwd_radius == dist_max && ! pruned) { break; } //bwd search done
            /*
            std::cerr<<"src="<< src <<" dst="<< dst
                     <<" frad="<< fwd_radius <<" brad="<< bwd_radius
                     <<" d="<< cur_dist_src_dst
                     <<" fq:"<< queue.size() <<" bq:"<< bwd_trav.queue.size()
                     <<"\n";
            */
            if ( ( ! pruned)
                && fwd_radius + bwd_radius >= cur_dist_src_dst) { break; }
        }

        return cur_dist_src_dst;
    }

    // Returns the current radius of the search : how far next node is from src
    dist bidir_dijkstra_step(const graph & g,
                             dist & cur_dist_src_dst, const dist dist_limit,
                             const trav & oth_trav, const node oth,
                             const dist oth_radius, // progr. of other search
                             std::function<bool(node, dist, node)> filter) {
        assert(oth_radius < dist_max);
        if (queue.empty()) { return dist_max; }
        node_dist ud;
        do {
            ud = queue.top(); queue.pop();
        } while (visited[ud._node] && ! queue.empty());
        node u = ud._node;
        if ( ! visited[u] ){ 
            dist du = ud._dist;
            assert(du == distances[u]);
            //std::cerr <<"bd_dijks: u="<< u <<" du="<< du <<" oth="<<oth<<"\n";
            visited[u] = true;
            visited_nodes.push_back(u);
            if (u == oth) { // at destination
                cur_dist_src_dst = du;
                return du;
            }
            if (du + oth_radius >= cur_dist_src_dst) {// cannot improve
                return du;
            }
            for (auto e : g.out_neighbors(u)) {
                node v = e.dst;
                dist dv = du + dist(e.len);
                // do we meet other traversal?
                dist d_v_oth = oth_trav.distances[v];
                if (d_v_oth < dist_max && dv + d_v_oth < cur_dist_src_dst) {
                    cur_dist_src_dst = dv + d_v_oth;
                }
                // Continue searching:
                if (filter(v, dv, u) && dv < distances[v]
                    && dv + oth_radius < std::min(cur_dist_src_dst, dist_limit)
                    ) {
                    distances[v] = dv;
                    queue.push(node_dist(v, dv));
                }
            }
            return du;
        }
        assert(queue.empty());
        return dist_max; // no more nodes
    }

};


namespace unit {
    void test_traversal();
}
