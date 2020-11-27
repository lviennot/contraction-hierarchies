// Identifier type based on integer type I.
// Using different type parameter T allows to make incompatible clones of I.

#include <cassert>
#include <string>

template<typename I, typename T>
class uint_index {
public:
    using uint_t = I;
    using index = uint_index<I, T>;
    static constexpr uint_t invalid_id = std::numeric_limits<uint_t>::max(); 
    explicit uint_index(uint_t i) : _i(i) {}
    uint_index() : _i(invalid_id) {}
    operator uint_t() const { return _i; }
    uint_t operator++() { return ++_i; }
    uint_t operator--() { return --_i; }
    // allow i+1 and i-1:
    using uint = unsigned int;
    index operator+(uint && i) {  assert(i == 1u); return index(_i + i); } 
    index operator-(uint && i) {  assert(i == 1u); return index(_i - i); } 
    index operator=(index o) { _i = o._i; return *this; }
    bool valid() const { return _i != invalid_id; }

    static void unit_test() {
        // Check uint_t is unsigned
        uint_t z = 0u;
        CHECK(z - 1u == std::numeric_limits<uint_t>::max());
    }
private:
    uint_t _i;
};


namespace std {

    // make it hashable:
    template<typename I, typename T>
    struct hash<uint_index<I,T>> {
        std::size_t operator()(const uint_index<I,T> & i) const {
            return (size_t)i;
        }
    };

    // make it stringable:
    template<typename I, typename T>
    string to_string(const uint_index<I,T> & i) {
        return to_string(I(i));
    }
}


