#pragma once

#include <random>

namespace aeh
{

    struct random_device_seed_sequence
        : std::random_device
    {
        template <typename It>
        auto generate(It first, It last) -> void
        {
            for (auto it = first; it != last; ++it)
                *it = (*this)();
        }
    };

    template <typename T>
    auto randomly_seeded() -> T
    {
        random_device_seed_sequence seed;
        return T(seed);
    }

} // namespace aeh
