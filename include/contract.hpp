#include <string_view>
#include <source_location>
#include <vector>
#include <functional>

#define contract contract_t _{};
#define pre(...) _.precondition([&]{ return __VA_ARGS__; }, #__VA_ARGS__);
#define cond(...) _.condition([&]{ return __VA_ARGS__; }, #__VA_ARGS__);
#define post(...) _.postcondition([&]{ return __VA_ARGS__; }, #__VA_ARGS__);
#define old(x) auto old_#x = x;

// a contract will generally look like this for the simple case:
/*
struct nonzero
{
    int value;
    nonzero(int new_value)
    {
        contract;
            pre(new_value != 0);
            post(value == new_value);

        value = new_value;
    };

    auto operator +=(int addend) -> nonzero&
    {
        old(value);
        contract;
            pre(value + addend != 0);
            post(value == old_value + addend);

        value += addend;
        return *this;
    };
};
*/

// non-macro versions can be done to provide more detailed labels and old values.
/*
auto func(double value, double* ptr) -> double
{
    auto old_ptr_value = *ptr;
    auto _ = contract_t{}
        .precondition([&]{ return ptr != nullptr; }, "ptr is non-null.")
        .precondition([&]{ return value > 0.0; }, "value > 0.")
        .postcondition([&]{ return *ptr == old_ptr_value + value; }, "value is added to *ptr.");

    *ptr += value;
    return old_ptr_value;
};
*/

struct contract_t
{
    using function_t = std::function<bool()>;

    struct postcondition_t
    {
        function_t function;
        std::string label;
        std::source_location source;
    };
    std::vector<postcondition_t> postconditions = {};
    std::uint32_t precondition_count = 0;
    std::uint32_t condition_count = 0;
    std::uint32_t postcondition_count = 0;

    contract_t() = default;
    contract_t(contract_t&) noexcept;
    contract_t(contract_t&&) noexcept;
    ~contract_t();

    auto precondition(const function_t& precondition, std::string_view label = {}, std::source_location source = std::source_location::current()) -> contract_t&;
    auto condition(const function_t& condition, std::string_view label = {}, std::source_location source = std::source_location::current()) -> contract_t&;
    auto postcondition(const function_t& postcondition, std::string_view label = {}, std::source_location source = std::source_location::current()) -> contract_t&;
};