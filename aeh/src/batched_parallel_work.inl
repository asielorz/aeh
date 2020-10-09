namespace aeh
{
	namespace detail
	{
		template <typename T>
		struct Batch
		{
			std::unique_ptr<T[]> results = nullptr;
			std::atomic<size_t> completed_work = 0;
		};

		template <typename T>
		struct Work
		{
			std::vector<Batch<T>> batches;
			std::mutex batch_allocation_mutex;
			size_t last_allocated_batch;

			Work(size_t batch_count)
				: batches(batch_count)
				, last_allocated_batch(0)
			{}

			Batch<T> & batch_at(size_t i, size_t batch_size)
			{
				if (batches[i].results)
					return batches[i];

				auto const g = std::lock_guard(batch_allocation_mutex);
				if (batches[i].results)
				{
					return batches[i];
				}

				batches[i].results = std::make_unique<T[]>(batch_size);
				last_allocated_batch = i;
				return batches[i];
			}

			void free_batch(size_t i)
			{
				if (last_allocated_batch == batches.size() - 1)
				{
					batches[i].results = nullptr;
				}
				else
				{
					batch_allocation_mutex.lock();
					auto const it = std::find_if(batches.begin() + last_allocated_batch + 1, batches.end(), [](Batch<T> const & b) { return b.results == nullptr; });
					if (it == batches.end())
					{
						batch_allocation_mutex.unlock();
						batches[i].results = nullptr;
					}
					else
					{
						last_allocated_batch = it - batches.begin();
						it->results = std::move(batches[i].results);
						batch_allocation_mutex.unlock();
					}
				}
			}
		};

		template <typename T, typename Task>
		bool perform_task(std::atomic<size_t> & index, size_t total_work, size_t batch_size, Work<T> & work, Task const & task)
		{
			size_t const next_task = index++;
			if (next_task >= total_work)
				return false;

			size_t const batch_index = next_task / batch_size;
			size_t const task_index_in_batch = next_task % batch_size;
			Batch<T> & batch = work.batch_at(batch_index, batch_size);
			batch.results[task_index_in_batch] = task(next_task);
			batch.completed_work++;
			return true;
		}

		template <typename T, typename Task>
		void worker_thread(std::atomic<size_t> & index, size_t total_work, size_t batch_size, Work<T> & work, Task const & task)
		{
			while (perform_task(index, total_work, batch_size, work, task)) {}
		}

	} // namespace detail

	template <typename Task, typename Process, typename Executor>
	void batched_parallel_work(size_t total_work, size_t batch_size, size_t worker_count, Task const & task, Process const & process, Executor && executor)
	{
		size_t const excedent = total_work % batch_size;
		size_t const batch_count = total_work / batch_size;

		using task_result_t = decltype(task(total_work));
		detail::Work<task_result_t> work(batch_count + (excedent > 0));

		std::atomic<size_t> index = 0;
		using worker_t = decltype(executor(detail::worker_thread<task_result_t, Task>, ref(index), total_work, batch_size, std::ref(work), std::ref(task)));
		std::vector<worker_t> workers;
		workers.reserve(worker_count);
		for (size_t i = 0; i < worker_count; ++i)
			workers.push_back(executor(detail::worker_thread<task_result_t, Task>, ref(index), total_work, batch_size, std::ref(work), std::ref(task)));

		size_t batches_processed = 0;
		while (batches_processed < batch_count)
		{
			detail::perform_task(index, total_work, batch_size, work, task);
			while (batches_processed < batch_count && work.batches[batches_processed].completed_work == batch_size)
			{
				process(work.batches[batches_processed].results.get(), batch_size);
				work.free_batch(batches_processed);
				batches_processed++;
			}
		}

		// Last batch, which is smaller and so has to be treated differently.
		if (excedent > 0)
		{
			detail::Batch<task_result_t> & last_batch = work.batches.back();
			while (last_batch.completed_work != excedent)
				detail::perform_task(index, total_work, batch_size, work, task);

			process(last_batch.results.get(), excedent);
		}

		for (worker_t & t : workers)
			t.join();
	}

} // namespace aeh
