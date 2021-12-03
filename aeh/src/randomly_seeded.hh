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

    template <typename RNG>
    struct rng_seed_sequence
    {
        RNG & rng;

        template <typename It>
        auto generate(It first, It last) -> void
        {
            for (auto it = first; it != last; ++it)
                *it = rng();
        }
    };

    template <typename OutputRNG, typename InputRNG>
    auto split_rng(InputRNG & rng) -> OutputRNG
    {
        auto seed = rng_seed_sequence(rng);
        return OutputRNG(seed);
    }

    // 64 bit RNG described by Knuth. Very small and very fast. Quality of generated numbers is not that good.
    // Use for things where a lot of random numbers are needed, and their quality at beating statistical tests
    // is not that important, like animating particles.
    // Obtained from: https://www.pcg-random.org/posts/cpp-seeding-surprises.html
    using knuth_lcg = std::linear_congruential_engine<uint64_t, 6364136223846793005U, 1442695040888963407U, 0U>;

} // namespace aeh
