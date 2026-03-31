#include <cmath>
#include <vector>
#include <soloud/soloud.h>

#include "PitchShiftFilter.hpp"

namespace Sound {
    typedef int32_t i32;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::vector, std::min, std::max, std::pow, std::copy;
    
    //SoLoud asks us to just `new` and return the pointer. No `delete` required. A truly brave paradigm.
    SoLoud::FilterInstance* PitchShiftFilter::createInstance() noexcept { return new PitchShiftFilterInstance(this); }

    PitchShiftFilterInstance::PitchShiftFilterInstance(PitchShiftFilter* parent) noexcept : parent(parent) {}

    constexpr double PI = 3.141592653589793;
    constexpr float WINDOW_RANGE_MS = 20.0f, SEARCH_RANGE_MS = 10.0f, OVERLAP_FRAC = 0.5f;

    inline static void resample(const vector<float>& in, vector<float>& out) noexcept {
        const float ratio = static_cast<float>(in.size()) / out.size();
        for (u64 i = 0; i < out.size(); i++) {
            float index = i * ratio;
            u64 indexInt = static_cast<u64>(index);
            float indexFrac = index - indexInt, data = in[indexInt], outData = in[min(indexInt + 1, in.size() - 1)];
            out[i] = data + (outData - data) * indexFrac;
        }
    }

    inline static vector<float> wsola(const float* buffer, u64 sampleCount, float sampleRate, float stretch) noexcept {
        const u64 windowLength = max<u64>(64, min<u64>(sampleCount, WINDOW_RANGE_MS * 1e-3f * sampleRate)), hopIntervalI = max<u64>(1, windowLength * (1.0f - OVERLAP_FRAC)), hopIntervalO = hopIntervalI * stretch;
        vector<float> window(windowLength);
        const float coeff = 2.0f * static_cast<float>(PI) / windowLength;
        for (u64 i = 0; i < windowLength; i++) window[i] = 0.5f - 0.5f * cos(coeff * i);
        const u64 outN = static_cast<u64>(sampleCount * stretch) + windowLength;
        vector<float> result(outN, 0.0f), ola(outN, 0.0f);
        u64 ip = 0, op = 0;
        while (ip + windowLength < sampleCount) {
            u64 best = ip;
            float bestCorr = -1e9f;
            const u64 search = static_cast<u64>(SEARCH_RANGE_MS * 1e-3f * sampleRate), lo = (ip > search) ? ip - search : 0, hi = min(ip + search, sampleCount - windowLength);
            for (u64 cand = lo; cand <= hi; cand += 4) {
                float corr = 0.0f;
                for (u64 i = 0; i < windowLength; i++) corr += buffer[ip + i] * buffer[cand + i];
                if (corr > bestCorr) {
                    bestCorr = corr;
                    best = cand;
                }
            }
            for (u64 i = 0; i < windowLength; i++) {
                result[op + i] += buffer[best + i] * window[i];
                ola[op + i] += window[i];
            }
            ip += hopIntervalI;
            op += hopIntervalO;
            if (op + windowLength >= result.size()) break;
        }
        for (u64 i = 0; i < result.size(); i++) if (ola[i] > 1e-6f) result[i] /= ola[i];
        return result;
    }

    void PitchShiftFilterInstance::filterChannel(float* buffer, u32 sampleCount, float sampleRate, double playTime, u32 currentChannel, u32 channelCount) noexcept {
        if (!buffer || sampleCount == 0 || parent->pitch == 1.0f) return;
        vector<float> stretched = wsola(buffer, sampleCount, sampleRate, 1.0f / pow(2.0f, parent->pitch)), finalBuf(sampleCount);
        resample(stretched, finalBuf);
        copy(finalBuf.begin(), finalBuf.end(), buffer);
    }
}