#pragma once
#include <iostream>

namespace compiler
{

struct Position
{
    int32_t line{ 1 };
    int32_t column{ 1 };

    bool operator==(Position const& rhs) const = default;
    void advance(char c);
};

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
    Loc() = default;
    explicit Loc(std::string_view filename) : filename_{ filename } {}
    Loc(std::string_view filename, int32_t line, int32_t col) : filename_{ filename }, position_{ line, col } {}

    bool operator==(Loc const& rhs) const { return filename_ == rhs.filename_ && position_ == rhs.position_; }

    static bool has_error() { return error_occured; }

    std::string_view filename() const { return filename_; }
    std::string format() const;

    Err err() const { return Err{ *this }; }
    Wrn wrn() const { return Wrn{ *this }; }

    void advance(char c) { position_.advance(c); }

    Position pos() const { return position_; }

private:
    std::string_view filename_{ "UNKNOWN FILE" };
    Position position_;
};

} // namespace compiler
