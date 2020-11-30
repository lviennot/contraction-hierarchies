#include <iostream>

// Saturated ints based on integer type I (designed for unsigned).
template<typename I>
class saturated_uint {
public:
    using int_t = I;
    using sat_t = saturated_uint<I>;
    static constexpr int_t infinity = std::numeric_limits<int_t>::max(); 
    explicit saturated_uint(std::uint64_t i) : _i(i) { assert(i <= infinity); }
    explicit saturated_uint(std::int64_t i) : _i(i) { assert(i <= infinity); }
    saturated_uint(int_t i) : _i(i) {}
    saturated_uint(int i) : _i(i) { assert(i >= 0); }
    saturated_uint() : _i(infinity) {}
    operator int_t() const { return _i; }
    sat_t operator=(sat_t o) { return _i = o._i; }
    sat_t operator+(sat_t o) { return operator+(o._i); }
    sat_t operator+(int_t i) {
        int_t r = _i + i;
        if(r < _i || r == infinity) {
            std::cerr << "saturated_uint.hh: overflow\n" << std::flush;
            std::abort();
        } else { return r; }
    }
    sat_t operator+(int && i) { return *this + sat_t(i); } // allow s + 20, not s + i
    bool saturated() const { return _i == infinity; }    
    bool finite() const { return _i != infinity; }    
private:
    int_t _i;
};

// Make its max accessible in standard way:
namespace std {
    template<typename I>
    struct numeric_limits<saturated_uint<I>> {
        static saturated_uint<I> max() {
            return saturated_uint<I>::infinity;
        }
    };
}
