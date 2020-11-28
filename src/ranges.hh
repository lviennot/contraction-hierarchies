// Basic ranges.

#pragma once

namespace utl { //  utils

template<typename I, const bool incr = true>
class int_iterator {
    I i;
public:
    int_iterator(I i) : i(i) {}
    I operator*() const { return i; }
    int_iterator &operator++() { if(incr) ++i; else --i; return *this; }
    bool operator!=(const int_iterator& o) { return i != o.i; }
    void decr() { --i; }
};

template<typename I>
class irange {
public:
    using iterator = int_iterator<I, true>;
    irange(I beg, I end) : _beg(beg), _end(end) {} // from _beg to _end-1
    iterator begin() const { return _beg; }
    iterator end() const { return _end; }
private:
    const iterator _beg, _end;
};

template<typename I>
class irange_rev {
public:
    using iterator = int_iterator<I, false>;
    irange_rev(I beg, I end) : _beg(end-1u), _end(beg-1u) {} // _end-1 to _beg
    iterator begin() const { return _beg; }
    iterator end() const { return _end; }
private:
    const iterator _beg, _end;
};

template<typename C, // container type
         typename iterator = typename C::const_iterator> // or C::iterator 
class crange {
public:
    crange(iterator beg, iterator end) : _beg(beg), _end(end) {};
    iterator begin() const { return _beg; }
    iterator end() const { return _end; }
private:
    const iterator _beg, _end;
};

}
