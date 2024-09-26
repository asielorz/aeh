#include "batched_parallel_work.hh"
#include <catch2/catch_test_macros.hpp>
#include <optional>

namespace tests
{

    static std::optional<size_t> filter_out_evens(size_t i)
    {
        if (i % 2 == 0)
            return std::nullopt;
        else
            return i;
    }

    template <typename Container>
    static auto push_back_to_if_valid(Container & c)
    {
        return [&c](std::optional<typename Container::value_type> results[], size_t result_count)
        {
            for (size_t i = 0; i < result_count; ++i)
                if (results[i].has_value())
                    c.push_back(std::move(*results[i]));
        };
    }

    struct worker_on_strike
    {
        void join() const {}
    };
    constexpr auto do_nothing = [](auto && ...) { return worker_on_strike(); };

} // namespace tests

TEST_CASE("Work is executed and the results processed")
{
    constexpr size_t task_count = 100000;
    constexpr size_t batch_size = 100;

    std::vector<size_t> outputs;

    aeh::batched_parallel_work(
        task_count, batch_size, std::thread::hardware_concurrency() - 1,
        tests::filter_out_evens, tests::push_back_to_if_valid(outputs),
        tests::do_nothing); // Use a dummy executor to avoid creating new threads

    for (size_t i : outputs)
        CHECK(i % 2 != 0);
}
