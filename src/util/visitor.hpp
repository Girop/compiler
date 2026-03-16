#pragma once


template <typename... T> struct Overload : T...
{
    using T::operator()...;
};

