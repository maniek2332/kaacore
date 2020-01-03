#pragma once

#include <limits>
#include <random>

namespace kaacore {

template<class T, class M>
static inline constexpr ptrdiff_t
offset_of(const M T::*member)
{
    return reinterpret_cast<ptrdiff_t>(&(reinterpret_cast<T*>(0)->*member));
}

template<class T, class M>
static inline constexpr T*
container_of(const M* ptr, const M T::*member)
{
    return reinterpret_cast<T*>(
        reinterpret_cast<intptr_t>(ptr) - offset_of(member));
}

template<typename T>
T
random_uid()
{
    static std::random_device rd;
    static std::default_random_engine generator{rd()};
    static std::uniform_int_distribution<T> distribution{
        std::numeric_limits<T>::min() + 1, std::numeric_limits<T>::max()};
    return distribution(generator);
}

} // namespace kaacore
