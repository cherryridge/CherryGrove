#pragma once
#include <iostream>
#include <sstream>
#include <mutex>
#include <thread>
#include <unordered_map>

//Auto use magic variables
using std::endl, std::flush;

namespace Logger {
	using std::cout, std::cerr, std::ostream, std::enable_if, std::is_function, std::stringstream, std::this_thread::get_id, std::lock_guard, std::mutex, std::unordered_map, std::string, std::thread;

	void shutdown();

	bool getMode();
	void setToFile(bool _toFile);

	extern mutex loggerMutex;
	extern unordered_map<thread::id, string> threadNames;

	extern thread_local stringstream threadBufferOdi;
	struct LoggerCout {
		LoggerCout& operator<<(ostream& (*manip)(ostream&)) {
			if (manip == static_cast<ostream& (*)(ostream&)>(endl)) {
				//Use l* << <Content> << endl to output thread-safe content.
				//l* << endl will result in nothing.
				if (!threadBufferOdi.str().empty()) {
					lock_guard guard(loggerMutex);
					cout << threadBufferOdi.str() << endl << flush;
					threadBufferOdi.str("");
					threadBufferOdi.clear();
				}
			}
			else if (manip == static_cast<ostream& (*)(ostream&)>(flush)) {
				//Use l* << <Name> << flush to set a customized name for this thread.
				if (!threadBufferOdi.str().empty()) {
					lock_guard guard(loggerMutex);
					string tBstr = threadBufferOdi.str(), name = tBstr.substr(tBstr.find_first_of(' ') + 1);
					auto p = threadNames.find(get_id());
					if (p == threadNames.end()) threadNames.emplace(get_id(), name);
					else p->second = name;
					threadBufferOdi.str("");
					threadBufferOdi.clear();
				}
				//Use l* << flush to delete the customized thread name.
				else {
					auto p = threadNames.find(get_id());
					if (p != threadNames.end()) threadNames.erase(p);
				}
			}
			else {
				if (threadBufferOdi.str().empty()) {
					auto p = threadNames.find(get_id());
					if (p == threadNames.end()) threadBufferOdi << "[" << get_id() << "] ";
					else threadBufferOdi << "[" << p->second << "] ";
				}
				threadBufferOdi << manip;
			}
			return *this;
		}

		template <typename T>
		typename enable_if<!is_function<T>::value, LoggerCout&>::type operator<<(const T& value) {
			if (threadBufferOdi.str().empty()) {
				auto p = threadNames.find(get_id());
				if (p == threadNames.end()) threadBufferOdi << "[" << get_id() << "] ";
				else threadBufferOdi << "[" << p->second << "] ";
			}
			threadBufferOdi << value;
			return *this;
		}

	};
	extern LoggerCout lout;

	extern thread_local stringstream threadBufferErr;
	struct LoggerCerr {
		LoggerCerr& operator<<(ostream& (*manip)(ostream&)) {
			if (manip == static_cast<ostream & (*)(ostream&)>(endl)) {
				//Use l* << <Content> << endl to output thread-safe content.
				//l* << endl will result in nothing.
				if (!threadBufferErr.str().empty()) {
					lock_guard guard(loggerMutex);
					//No need to flush because `cerr` will automatically empty the buffer to the screen.
					cerr << threadBufferErr.str() << endl;
					threadBufferErr.str("");
					threadBufferErr.clear();
				}
			}
			else if (manip == static_cast<ostream & (*)(ostream&)>(flush)) {
				//Use l* << <Name> << flush to set a customized name for this thread.
				if (!threadBufferErr.str().empty()) {
					lock_guard guard(loggerMutex);
					string tBstr = threadBufferErr.str(), name = tBstr.substr(tBstr.find_first_of(' ') + 1);
					auto p = threadNames.find(get_id());
					if (p == threadNames.end()) threadNames.emplace(get_id(), name);
					else p->second = name;
					threadBufferErr.str("");
					threadBufferErr.clear();
				}
				//Use l* << flush to delete the customized thread name.
				else {
					auto p = threadNames.find(get_id());
					if (p != threadNames.end()) threadNames.erase(p);
				}
			}
			else {
				if (threadBufferErr.str().empty()) {
					auto p = threadNames.find(get_id());
					if (p == threadNames.end()) threadBufferErr << "[" << get_id() << "] ";
					else threadBufferErr << "[" << p->second << "] ";
				}
				threadBufferErr << manip;
			}
			return *this;
		}

		template <typename T>
		typename enable_if<!is_function<T>::value, LoggerCerr&>::type operator<<(const T& value) {
			if (threadBufferErr.str().empty()) {
				auto p = threadNames.find(get_id());
				if (p == threadNames.end()) threadBufferErr << "[" << get_id() << "][Error] ";
				else threadBufferErr << "[" << p->second << "][Error] ";
			}
			threadBufferErr << value;
			return *this;
		}

	};
	extern LoggerCerr lerr;
}

//Auto use magic variables
using Logger::lout, Logger::lerr;