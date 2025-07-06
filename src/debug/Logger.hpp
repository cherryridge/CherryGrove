#pragma once
#include <atomic>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <thread>
#include <type_traits>

//Auto use magic variables
using std::endl, std::flush;

namespace Logger {
    using std::atomic_flag, std::ostream, std::enable_if, std::is_function, std::ostringstream, std::memory_order_acquire, std::memory_order_release, std::string, std::this_thread::get_id;
    using Manip = ostream& (*)(ostream&);

    void setToFile(bool _toFile) noexcept;
    void shutdown() noexcept;

    struct Logger {
        explicit Logger(ostream& output, const string& title = string(), bool immediateFlush = false) noexcept : output(output), immediateFlush(immediateFlush) {
            if (title != "") this->title = "[" + title + "] ";
        }

        Logger& operator<<(Manip manip) noexcept {
            if (manip == static_cast<Manip>(endl) && !buffer.str().empty()) {
                while (flag.test_and_set(memory_order_acquire)) {}
                if (threadName != "") output << threadName;
                else output << "[" << get_id() << "]";
                if (title != "") output << title;
                else output << " ";
                output << buffer.str() << "\n";
                flushBuffer();
                if (immediateFlush) output.flush();
                flag.clear(memory_order_release);
            }
            else if (manip == static_cast<Manip>(flush)) {
                //Use instance << flush to delete the customized thread name.
                if (buffer.str().empty()) threadName = "";
                else {
                    threadName = "[" + buffer.str() + "]";
                    flushBuffer();
                }
            }
            //Don't react to other manipulation functions.
            //else
            return *this;
        }

        template<typename T>
        typename enable_if<!is_function<T>::value, Logger&>::type operator<<(const T& value) noexcept {
            buffer << value;
            return *this;
        }

    private:
        ostream& output;
        atomic_flag flag{};
        string title{};
        bool immediateFlush;
        //We have to use `static` on buffer because C++ only allow thread_local entries with static or external linkage.
        //And this implies we need to flush the buffer every time we finish the
        static thread_local ostringstream buffer;
        static thread_local string threadName;

        void flushBuffer() noexcept {
            buffer.str({});
            buffer.clear();
        }
    };

    inline thread_local ostringstream Logger::buffer;
    inline thread_local string Logger::threadName;

    extern Logger lout, lerr;
}

using Logger::lout, Logger::lerr;