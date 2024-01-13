#include "contract.hpp"

#include <spdlog/spdlog.h>

enum class condition_type
{
    precondition,
    condition,
    postcondition,
};

auto handle_failure(condition_type type, std::uint32_t which, std::string_view label, const std::source_location& source)
{
    switch (type)
    {
    using enum condition_type;
    case precondition:
        spdlog::error("precondition #{} failed in {}: {}", which, source.function_name(), label);
        break;
    case condition:
        spdlog::error("condition #{} failed in {}: {}", which, source.function_name(), label);
        break;
    case postcondition:
        spdlog::error("postcondition #{} failed in {}: {}", which, source.function_name(), label);
        break;
    default:
        break;
    }
};

contract_t::contract_t(contract_t& other) noexcept :
    contract_t{}
{
    std::swap(postconditions, other.postconditions);
    std::swap(precondition_count, other.precondition_count);
    std::swap(condition_count, other.condition_count);
    std::swap(postcondition_count, other.postcondition_count);
};

contract_t::contract_t(contract_t&& other) noexcept :
    contract_t{}
{
    std::swap(postconditions, other.postconditions);
    std::swap(precondition_count, other.precondition_count);
    std::swap(condition_count, other.condition_count);
    std::swap(postcondition_count, other.postcondition_count);
};

contract_t::~contract_t()
{
    for (const postcondition_t& postcondition : postconditions)
    {
        if (not postcondition.function())
        {
            handle_failure(condition_type::postcondition, postcondition_count, postcondition.label, postcondition.source);
        }

        ++postcondition_count;
    }
};

auto contract_t::precondition(const function_t& precondition, std::string_view label, std::source_location source) -> contract_t&
{
    if (not precondition())
    {
        handle_failure(condition_type::precondition, precondition_count, label, source);
    }

    ++precondition_count;
    return *this;
};

auto contract_t::condition(const function_t& condition, std::string_view label, std::source_location source) -> contract_t&
{
    if (not condition())
    {
        handle_failure(condition_type::condition, condition_count, label, source);
    }

    ++condition_count;
    return *this;
};

auto contract_t::postcondition(const function_t& postcondition, std::string_view label, std::source_location source) -> contract_t&
{
    postconditions.emplace_back(postcondition, std::string{ label }, source);

    return *this;
};