#pragma once
#include <mutex>
#include <map>
#include <ctime>

#include "saveBase.hpp"

namespace Save {
    extern std::map<time_t, WorldInfo> worldList;
    extern std::mutex worldListMutex;

    void refreshWorldList(const char* rootDir = "saves");
}