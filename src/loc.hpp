#pragma once
#include <cstddef>
#include <iostream>

namespace compiler
{

class Loc
{
    inline static bool error_occured{ false };

    template <typename Self> class Diagnostic
    {
    public:
        explicit Diagnostic(Loc const& loc) : loc_{ loc } {}

        template <typename T> std::ostream& operator<<(T&& message)
        {
            auto& self{ static_cast<Self&>(*this) };
            error_occured = error_occured || self.is_terminating();
            return std::cerr << self.type() << loc_.format() << " " << message;
        }

    private:
        Loc const& loc_;
    };

    class Wrn : public Diagnostic<Wrn>
    {
        friend Diagnostic;
        using Diagnostic::Diagnostic;
        constexpr std::string_view type() const { return "Warning: "; }
        constexpr bool is_terminating() { return false; }
    };

    class Err : public Diagnostic<Err>
    {
        friend Diagnostic;
        using Diagnostic::Diagnostic;
        constexpr std::string_view type() const { return "Error: "; }
        constexpr bool is_terminating() { return true; }
    };

public:
    explicit Loc(std::string_view filename) : filename_{ filename } {}

    Loc(std::string_view filename, size_t row, size_t col) : filename_{ filename }, row_{ row }, column_{ col } {}

    static bool has_error() { return error_occured; }

    std::string_view filename() const { return filename_; }
    std::string format() const;

    Err err() const { return Err{ *this }; }
    Wrn wrn() const { return Wrn{ *this }; }

    void advance(char c);

private:
    std::string_view filename_;
    size_t row_{ 1 };
    size_t column_{ 1 };
};

} // namespace compiler
