#pragma once

#include <condition_variable>
#include <deque>
#include <limits>
#include <mutex>

namespace Core
{
	template<typename ValueType>
	class Queue
	{
	public:
		Queue() { this->maxSize = 1024;  }; // : maxSize(std::numeric_limits<std::size_t>::max()) {}
		//Queue(std::string name) : name(name), maxSize(std::numeric_limits<std::size_t>::max()) {}
		explicit Queue(std::size_t maxSize) : maxSize(maxSize) {}
		explicit Queue(std::string name, std::size_t maxSize) : name(name), maxSize(maxSize) {}

		void send(const ValueType& toSend)
		{
			std::lock_guard<std::mutex> guard(key);
			nonFull.wait(key, [this] {
				return fifo.size() <= maxSize;
			});
			fifo.push_back(toSend);
			nonEmpty.notify_all();
		}

		bool trySend(const ValueType& toSend)
		{
			bool sent = false;
			if (key.try_lock())
			{
				std::lock_guard<std::mutex> guard(key, std::adopt_lock);
				if (fifo.size() <= maxSize)
				{
					fifo.push_back(toSend);
					nonEmpty.notify_all();
					sent = true;
				}
			}
			return sent;
		}

		void receive(ValueType& toReceive)
		{
			std::lock_guard<std::mutex> guard(key);
			nonEmpty.wait(key, [this] {
				return !fifo.empty();
			});
			toReceive = fifo.front();
			fifo.pop_front();
			nonFull.notify_all();
		}

		bool tryReceive(ValueType& toReceive)
		{
			bool received = false;
			if (key.try_lock())
			{
				std::lock_guard<std::mutex> guard(key, std::adopt_lock);
				if (!fifo.empty())
				{
					toReceive = fifo.front();
					fifo.pop_front();
					nonFull.notify_all();
					received = true;
				}
			}
			return received;
		}

		size_t size()
		{
			return fifo.size();
		}

	private:
		std::mutex key;
		std::condition_variable_any nonEmpty;
		std::condition_variable_any nonFull;
		std::string name;
		std::size_t maxSize;
		std::deque<ValueType> fifo;
	};
}

// Kevlin Henney's implementation from NDC Conferences:
// Thinking Outside the Synchronisation Quadrant - Kevlin Henney
// Source: https://www.youtube.com/watch?v=2yXtZ8x7TXw
