#include "type.hpp"
#include <algorithm>
#include <array>

namespace compiler::ast
{

using tokens::Keyword;

namespace
{

Type::Qualifiers qual(std::vector<tokens::Keyword> const& keywords)
{
    Type::Qualifiers quals;

    for (auto& k : keywords)
    {
        switch (k)
        {
        case Keyword::Const: quals.set(to_underlying(Qualifier::Const)); continue;
        case Keyword::Volatile: quals.set(to_underlying(Qualifier::Volatile)); continue;
        case Keyword::Restrict: quals.set(to_underlying(Qualifier::Restrict)); continue;
        default: continue;
        }
    }
    return quals;
}

Storage storage(std::vector<tokens::Keyword> const& keywords)
{
    Storage storage{ Storage::Unspecified };
    for (auto& k : keywords)
    {
        switch (k)
        {
        case Keyword::Extern: storage = Storage::Extern; break;
        case Keyword::Auto: storage = Storage::Auto; break;
        case Keyword::Static: storage = Storage::Static; break;
        case Keyword::Register: storage = Storage::Register; break;
        default: continue;
        }
    }
    return storage;
}

class TypeDeductionTable
{
public:
    enum class State : uint8_t
    {
        Error = 0,
        Inital,
        Void,
        Bool,
        Char,
        Float,
        Int,
        Short,
        ShortInt,
        Long,
        LongLong,
        LongInt,
        LongLongInt,
        LongDouble,
        Double,
        _Count
    };

private:
    using Storage = std::array<std::array<State, to_underlying(Keyword::Imaginary)>, to_underlying(State::_Count)>;

public:
    consteval TypeDeductionTable()
    {
        fill(State::Inital, Keyword::Char, State::Char);
        fill(State::Inital, Keyword::Void, State::Void);
        fill(State::Inital, Keyword::Bool, State::Bool);
        fill(State::Float, Keyword::Float, State::Float);
        fill(State::Inital, Keyword::Long, State::Long);

        fill(State::Inital, Keyword::Int, State::Int);

        fill(State::Long, Keyword::Int, State::LongInt);
        fill(State::Int, Keyword::Long, State::LongInt);
        fill(State::LongInt, Keyword::Long, State::LongLongInt);

        fill(State::Long, Keyword::Double, State::LongDouble);
        fill(State::Inital, Keyword::Double, State::Double);
        fill(State::Double, Keyword::Long, State::LongDouble);

        fill(State::Long, Keyword::Long, State::LongLong);
        fill(State::LongLong, Keyword::Int, State::LongLongInt);

        fill(State::Inital, Keyword::Short, State::Short);
        fill(State::Short, Keyword::Int, State::ShortInt);
        fill(State::Int, Keyword::Short, State::ShortInt);
    }

    constexpr State operator()(State state, Keyword keyword) const
    {
        return storage_.at(to_underlying(state)).at(to_underlying(keyword));
    }

    BasicType to_type(Loc loc, State state) const
    {
        switch (state)
        {
        case State::Char: return BasicType::SignedChar;
        case State::Float: return BasicType::Float;
        case State::Int: return BasicType::Int;
        case State::Void: return BasicType::Void;
        case State::Bool: return BasicType::Bool;
        case State::Short: return BasicType::ShortInt;
        case State::ShortInt: return BasicType::ShortInt;
        case State::Long: return BasicType::LongInt;
        case State::LongInt: return BasicType::LongInt;
        case State::LongLongInt: return BasicType::LongLongInt;
        case State::LongLong: return BasicType::LongLongInt;
        case State::LongDouble: return BasicType::LongDouble;
        case State::Double: return BasicType::Double;
        case State::Error:
        case State::Inital:
        case State::_Count: break;
        }

        loc.err() << "Invaid type specifier\n";
        return BasicType::Void;
    }

    constexpr bool ignorable(Keyword k) const
    {
        switch (k)
        {
        case Keyword::Const:
        case Keyword::Volatile:
        case Keyword::Restrict:
        case Keyword::Signed:
        case Keyword::Unsigned:
        case Keyword::Extern:
        case Keyword::Auto:
        case Keyword::Static:
        case Keyword::Register: return true;
        default: return false;
        }
    }

private:
    consteval void fill(State state, Keyword on, State value)
    {
        storage_.at(to_underlying(state)).at(to_underlying(on)) = value;
    }

    Storage storage_{};
} constexpr table;

BasicType basic_type(std::vector<tokens::Keyword> const& keywords, Loc loc)
{
    using State = TypeDeductionTable::State;
    State result{ State::Error };
    State state{ State::Inital };

    for (auto& k : keywords)
    {
        if (table.ignorable(k)) continue;
        state = table(state, k);
        if (state == State::Error) break;
        result = state;
    }
    return table.to_type(loc, result);
}

} // namespace

Type::Type(Loc loc, std::vector<tokens::Keyword>&& keywords) :
    Node(loc),
    quals_{ qual(keywords) },
    storage_{ storage(keywords) },
    basic_{ basic_type(keywords, loc) },
    signed_{ std::all_of(keywords.begin(), keywords.end(), [](auto& k) { return k != Keyword::Unsigned; }) }
{
}

void Type::set_default_storage(Storage storage)
{
    if (storage_ == Storage::Unspecified)
    {
        storage_ = storage;
    }
}

std::ostream& Type::stream(std::ostream& os) const
{
    std::string type;

    if (quals_.test(static_cast<int>(Qualifier::Const)))
    {
        type += "const ";
    }

    if (quals_.test(static_cast<int>(Qualifier::Volatile)))
    {
        type += "volatile ";
    }

    if (quals_.test(static_cast<int>(Qualifier::Restrict)))
    {
        type += "restrict ";
    }

    switch (storage_)
    {
    case Storage::Unspecified: REPORT_ICE("Type wihtout set storage");
    case Storage::Extern: type += "extern "; break;
    case Storage::Auto: type += "auto "; break;
    case Storage::Static: type += "static "; break;
    case Storage::Register: type += "register "; break;
    }

    if (!signed_)
    {
        type += "unsigned ";
    }

    switch (basic_)
    {
    case BasicType::SignedChar: type += "char "; break;
    case BasicType::ShortInt: type += "short int "; break;
    case BasicType::Int: type += "int "; break;
    case BasicType::LongInt: type += "long int "; break;
    case BasicType::LongLongInt: type += "long long int "; break;
    case BasicType::Float: type += "float "; break;
    case BasicType::Double: type += "double "; break;
    case BasicType::LongDouble: type += "long double "; break;
    case BasicType::Bool: type += "bool "; break;
    case BasicType::Void: type += "void "; break;
    }
    return os << type;
}

} // namespace compiler::ast
