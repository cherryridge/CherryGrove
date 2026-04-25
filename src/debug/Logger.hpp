#pragma once
#include <atomic>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <thread>
#include <type_traits>
#include <glaze/glaze.hpp>

#include "../globalState.hpp"
#include "../util/json/helpers.hpp"
#include "../util/os/thread.hpp"

//Auto use magic variables
using std::endl, std::flush, std::ends;

namespace Logger {
    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;
    //Warning: DO NOT put std::cout or std::cerr here, or we will eventually misuse them.
    using std::atomic_flag, std::ostream, std::cout, std::cerr, std::is_function_v, std::ostringstream, std::memory_order_acquire, std::memory_order_release, std::forward, std::string, std::to_string, std::this_thread::yield;
    using Manip = ostream& (*)(ostream&);

    enum struct LoggingMode : u8 { Stdout, Separate, File };

    void init(LoggingMode mode) noexcept;
    void shutdown() noexcept;

    struct Logger {
        explicit Logger(ostream* output, atomic_flag* outputFlag, const string& title = "", bool immediateFlush = false) noexcept : title(title), output(output), outputFlag(outputFlag), immediateFlush(immediateFlush) {}

        Logger& operator<<(Manip manip) noexcept {
            if ((manip == static_cast<Manip>(endl) || manip == static_cast<Manip>(ends)) && !buffer.str().empty()) {
                string temp;
                if (!title.empty()) temp += title;
                if (threadName != "") temp += threadName;
                else temp += "[" + to_string(Util::getThreadId()) + "] ";
                temp += buffer.str();
                flushBuffer();
                //Use `ends` for not adding a newline.
                if (manip == static_cast<Manip>(endl)) temp += "\n";
                while (outputFlag->test_and_set(memory_order_acquire)) yield();
                *output << temp;
                if (immediateFlush) output->flush();
                outputFlag->clear(memory_order_release);
            }
            else if (manip == static_cast<Manip>(flush)) {
                //Use instance << flush to delete the customized thread name.
                if (buffer.str().empty()) threadName = "";
                else {
                    threadName = "[" + buffer.str() + "] ";
                    flushBuffer();
                }
            }
            //Don't react to other manipulation functions.
            //else
            return *this;
        }

        template <typename T> requires (!is_function_v<T>)
        Logger& operator<<(const T& value) noexcept {
            buffer << value;
            return *this;
        }

        template <typename... Ts> requires (!is_function_v<Ts>, ...)
        Logger& print(Ts&&... values) noexcept {
            ((*this << forward<Ts>(values)), ...);
            *this << endl;
            return *this;
        }

        //Flushes the destination stream.
        void operator()() const noexcept { output->flush(); }

        void redirect(ostream* newOutput, atomic_flag* newOutputFlag) noexcept {
            if (newOutput != nullptr) {
                output->flush();
                flushBuffer();
                outputFlag = newOutputFlag;
                output = newOutput;
            }
        }

    private:
        const string title;
        ostream* output{nullptr};
        atomic_flag* outputFlag{nullptr};
        //Whether to flush the destination stream after each endl, not the internal buffer. The internal buffer is always flushed after each endl.
        const bool immediateFlush{false};
        //We have to use `static` on buffer because C++ only allow thread_local entries with static or external linkage.
        //And this implies we need to flush the buffer every time we finish one output, before releasing the flag.
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

    template <typename... Ts>
    inline void LOGGER_DYNAMIC_OUT(Ts&&... ts) noexcept {
        if (GlobalState::multiThreadEra()) {
            ((lout << forward<Ts>(ts)), ...);
            lout << endl;
        }
        else {
            ((cout << forward<Ts>(ts)), ...);
            cout << endl;
        }
    }

    template <typename... Ts>
    inline void LOGGER_DYNAMIC_ERR(Ts&&... ts) noexcept {
        if (GlobalState::multiThreadEra()) {
            ((lerr << forward<Ts>(ts)), ...);
            lerr << endl;
        }
        else {
            cerr << "(Error) ";
            ((cerr << forward<Ts>(ts)), ...);
            cerr << endl;
        }
    }
}


GLAZE_ENUM_START(Logger::LoggingMode)
    GLAZE_ENUM("stdout", Stdout),
    GLAZE_ENUM("separate", Separate),
    GLAZE_ENUM("file", File)
GLAZE_ENUM_END

using Logger::lout, Logger::lerr;