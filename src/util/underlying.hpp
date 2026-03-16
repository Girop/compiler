#pragma once
#include <type_traits>

namespace compiler
{

template <typename T, typename Underlying = std::underlying_type_t<T>> constexpr Underlying to_underlying(T e)
{
    return static_cast<Underlying>(e);
}

} // namespace compiler
