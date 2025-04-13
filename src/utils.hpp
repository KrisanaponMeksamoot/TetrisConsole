#pragma once

#define CH_TAB 9
#define CH_ENTER 10
#define CH_ESC 27
#define CH_SPACE 32

template<typename T>
constexpr inline bool inrange(T v, T a, T b) {
    return a <= v && v <= b;
}

template<typename T>
constexpr inline T mod(T a, T b) {
    T res = a % b;
    if (res < 0)
        return res + b;
    return res;
}