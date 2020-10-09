#include <atomic>
#include <vector>
#include <mutex>
#include <algorithm>
#include <thread>

namespace aeh
{

	constexpr auto run_in_new_thread = [](auto && f, auto && ... args) 
	{
		return std::thread(std::forward<decltype(f)>(f), std::forward<decltype(args)>(args)...); 
	};

	template <typename Task, typename Process, typename Executor = decltype(run_in_new_thread) const &>
	void batched_parallel_work(
		size_t total_work, size_t batch_size, size_t worker_count, 
		Task const & task, Process const & process, 
		Executor && executor = run_in_new_thread);

} // namespace aeh

#include "batched_parallel_work.inl"
