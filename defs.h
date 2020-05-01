#ifndef BASIL_DEFS_H
#define BASIL_DEFS_H

#include <cstdint>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

struct opcode {
    u8 a: 4, b: 4, c: 4, n: 4;
    opcode():
        n(0) {}
    opcode(u8 a_): 
        a(a_), b(1), c(1), n(1) {}
    opcode(u8 a_, u8 b_): 
        a(a_), b(b_), c(1), n(2) {}
    opcode(u8 a_, u8 b_, u8 c_): 
        a(a_), b(b_), c(c_), n(3) {}
    operator bool() const {
        return n;
    }
    bool operator==(opcode other) const {
        return other.a == a && other.b == b && other.c == c && other.n == n;
    }
    u16 ser() const {
        return a << 12 | (n > 1 ? b << 8 : 0) | (n > 2 ? c << 4 : 0);
    }
};

// util/hash.h

template<typename T>
class set;

template<typename K, typename V>
class map;

// util/io.h

class stream;
class file;
class buffer;

// util/str.h

class string;

// util/vec.h

template<typename T>
class vector;

#endif