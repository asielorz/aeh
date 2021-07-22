#include "flexible_immutable_array.hh"

namespace aeh::msp::detail
{

    struct the_empty_array_buffer_t
    {
        std::aligned_storage_t<sizeof(shared<flexible_immutable_array<int>>), alignof(shared<flexible_immutable_array<int>>)> buffer;
    };
    the_empty_array_buffer_t make_empty_flexible_immutable_array() noexcept
    {
        the_empty_array_buffer_t buffer;
        auto const new_array = std::launder(reinterpret_cast<shared<flexible_immutable_array<int>> *>(&buffer));
        new_array->reference_counter = 1; // Must never run out of references since it cannot be deleted because it's a global.
        new_array->value.size_ = 0;
        return buffer;
    }
    the_empty_array_buffer_t the_empty_array_buffer = make_empty_flexible_immutable_array();
    shared<flexible_immutable_array<int>> & the_empty_array = *std::launder(reinterpret_cast<shared<flexible_immutable_array<int>> *>(&the_empty_array_buffer));

} // aeh::msp::namespace detail
