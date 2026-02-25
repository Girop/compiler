#pragma once
#include <iostream>
#include <type_traits>

namespace compiler
{

template <typename T, typename Underlying = std::underlying_type_t<T>> constexpr Underlying to_underlying(T e)
{
    return static_cast<Underlying>(e);
}

} // namespace compiler

#define REPORT_ICE(MSG)                                                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
        std::cerr << "Internal compiler error occured: \n\tfile: " << __FILE__ << "\n\tFunction: " << __FUNCTION__     \
                  << "\n\tLine: " << __LINE__ << "\n\tMessage: " << MSG << '\n';                                       \
        std::exit(2);                                                                                                  \
    } while (0)
