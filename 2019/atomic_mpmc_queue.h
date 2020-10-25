#pragma once

#include <atomic>
#include <type_traits>

/** Stylistic and portability overhaul of https://www.github.com/spectre1989/fast_mpmc_queue. */

template <typename T, std::size_t N, std::size_t cache_line_size = 64>
class alignas(cache_line_size)atomic_mpmc_queue
{
	struct alignas(cache_line_size)item_type
	{
		std::atomic<std::size_t> version;
		T value;
	};

public:
	atomic_mpmc_queue();
	virtual ~atomic_mpmc_queue() { }

	// non-copyable
	// TODO Though, we should be able to move them
	atomic_mpmc_queue(const atomic_mpmc_queue&) = delete;
	atomic_mpmc_queue(const atomic_mpmc_queue&&) = delete;
	atomic_mpmc_queue& operator=(const atomic_mpmc_queue&) = delete;
	atomic_mpmc_queue& operator=(const atomic_mpmc_queue&&) = delete;

	bool try_enqueue(const T& value);
	bool try_dequeue(T& out);

	std::size_t capacity() const { return N; }
	void reset();

private:
	void init();

	// Items are automatically aligned on cache line boundaries due to aligned item struct.
	item_type m_items[N];

	// Make sure each index is on a different cache line
	alignas(cache_line_size)std::atomic<std::size_t> m_head;
	alignas(cache_line_size)std::atomic<std::size_t> m_tail;
};

template <typename T, std::size_t N, std::size_t cache_line_size>
atomic_mpmc_queue<T, N, cache_line_size>::atomic_mpmc_queue()
	: m_head(0), m_tail(0)
{
	init();
}

template <typename T, std::size_t N, std::size_t cache_line_size>
void atomic_mpmc_queue<T, N, cache_line_size>::reset()

{
	m_head = 0;
	m_tail = 0;
	init();
}

template <typename T, std::size_t N, std::size_t cache_line_size>
bool atomic_mpmc_queue<T, N, cache_line_size>::try_enqueue(const T& value)
{
	std::size_t tail = m_tail.load(std::memory_order_relaxed);

	if(m_items[tail % N].version.load(std::memory_order_acquire) != tail)
		return false;

	// TODO Replace with weak loop.
	if(!m_tail.compare_exchange_strong(tail, tail + 1, std::memory_order_relaxed))
		return false;

	m_items[tail % N].value = value;

	// Release operation, all reads/writes before this store cannot be reordered past it
	// Writing version to tail + 1 signals reader threads when to read payload
	m_items[tail % N].version.store(tail + 1, std::memory_order_release);

	return true;
}

template <typename T, std::size_t N, std::size_t cache_line_size>
bool atomic_mpmc_queue<T, N, cache_line_size>::try_dequeue(T& out)
{
	std::size_t head = m_head.load(std::memory_order_relaxed);

	// Acquire here makes sure read of m_data[head].value is not reordered before this
	// Also makes sure side effects in try_enqueue are visible here
	if(m_items[head % N].version.load(std::memory_order_acquire) != (head + 1))
		return false;

	// TODO Replace with weak loop.
	if(!m_head.compare_exchange_strong(head, head + 1, std::memory_order_relaxed))
		return false;

	out = m_items[head % N].value;

	// This signals to writer threads that they can now write something to this index
	m_items[head % N].version.store(head + N, std::memory_order_release);

	return true;
}

template <typename T, std::size_t N, std::size_t cache_line_size>
void atomic_mpmc_queue<T, N, cache_line_size>::init()
{
	std::size_t ndx = 0;
	for (auto& item : m_items)
	{
		item.version = ndx++;
		item.value = 0;
	}
}
