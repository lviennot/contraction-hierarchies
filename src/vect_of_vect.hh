// Author: Laurent Viennot, Inria, 2020.

/** Convenient class for a vector of vectors.
 */

#pragma once

#include <cassert>
#include <vector>

#include "range.hh"

template <typename I, // index: an integer type convertible to size_t
          typename E> // element type

class vect_of_vect {

public:

    using index = I;
    using element = E;

protected:
    
    std::vector<std::vector<element>> vectors;
    std::size_t _n; // number of vectors
    std::size_t _m; // total number of elements

public:

    vect_of_vect() : _n(0), _m(0) {}
    vect_of_vect(vect_of_vect && g)
        : _n(g._n), _m(g._m), vectors(std::move(g.vectors)) {}
    vect_of_vect(const vect_of_vect & g) : _n(0), _m(0) {
        vectors.reserve(g.nb_indexs());
        for (index u : g) {
            vectors[u].reserve(g.vect_size(u));
            for (auto e : g[u]) { add(u, e); }
        }
    }

    std::size_t nb_vect() const { return _n; }
    std::size_t total_size() const { return _m; }
    std::size_t vect_size(index u) const { return vectors[u].size(); }
    
    void add_index(index i) {
        if (i >= _n) {
            _n = std::size_t(i) + 1;
            vectors.resize(_n);
        }
    }

    // We don't check if the edge is already there (multi-vect_of_vect).
    void add(index src, element hd) {
        const index dst = hd;
        add_index(src);
        add_index(dst);
        vectors[src].push_back(hd);
        ++_m;
    }

    // iterator on indexes:
    int_iterator<index> begin() const { return int_iterator<index>(index(0)); }
    int_iterator<index> end() const { return int_iterator<index>(index(_n)); }

    
    using vrange = crange<typename std::vector<element>>;

    vrange operator[](index u) const {
        if (u >= 0 && u < _n) {
            return vrange(vectors[u].cbegin(), vectors[u].cend());
        } else {
            throw std::invalid_argument("vect_of_vect.hh: invalid index "
                                        + std::to_string(u));
        }
    }

};


