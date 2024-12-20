#pragma once
#include <iostream>
#include <sstream>
#include <mutex>
#include <thread>

using std::cout, std::endl, std::ostream, std::enable_if, std::is_function, std::ios, std::stringstream, std::this_thread::get_id, std::lock_guard, std::mutex, std::to_string;

namespace Logger {

	extern mutex coutMutex;
	extern thread_local stringstream threadBuffer;

	struct LoggerCout {
		LoggerCout& operator<<(ostream& (*endl)(ostream&)) {
			if (!threadBuffer.str().empty()) {
				lock_guard guard(coutMutex);
				cout << threadBuffer.str() << endl;
				threadBuffer.str("");
				threadBuffer.clear();
			}
			return *this;
		}

		template <typename T>
		typename enable_if<!is_function<T>::value, LoggerCout&>::type operator<<(const T& value) {
			auto* buffer = threadBuffer.rdbuf();
			if (threadBuffer.str().empty()) threadBuffer << "[" << get_id() << "] ";
			threadBuffer << value;
			return *this;
		}

	};

	extern LoggerCout lout;

	void shutdown();

	bool getMode();
	void setToFile(bool _toFile);
}