#include "Logger.h"
#include <sstream>

namespace Core
{
	struct LoggerQItem
	{
		bool quit = false;
		std::string threadTag;
		std::string message;
	};

	Queue<LoggerQItem> loggerQueue = Queue<LoggerQItem>();
	std::thread loggerThread;

	static void loggerFunc(Queue<LoggerQItem>& in)
	{
		std::ostringstream id;
		id << std::this_thread::get_id();
		std::string threadTag = std::format("(TID={}) ", id.str());

		std::cout << threadTag << "[Logger] started" << std::endl;
		while (true)
		{
			LoggerQItem qItem;
			in.receive(std::ref(qItem));
			if (qItem.quit) break;
			std::cout << qItem.threadTag << qItem.message << std::endl;
		}
		std::cout << threadTag << "[Logger] stopped" << std::endl;
	}

	void startLogger()
	{
		loggerThread = std::thread(loggerFunc, std::ref(loggerQueue));
	}

	void log(std::string message)
	{
		std::ostringstream id;
		id << std::this_thread::get_id();
		std::string threadTag = std::format("(TID={}) ", id.str());

		loggerQueue.send({ false, threadTag, message });
	}

	void stopLogger()
	{
		loggerQueue.send({ true });
		if (loggerThread.joinable())
		{
			loggerThread.join();
		}
	}
}
