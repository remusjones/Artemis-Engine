#pragma once
#include <chrono>
#include <thread>

#include "Profiler.h"

struct TimerMetadata {
    TimerMetadata(const char* aFunctionSignature, const int aLineNumber) : FunctionSignature(aFunctionSignature), LineNumber(aLineNumber) {}
    TimerMetadata();

    const char* FunctionSignature;
    const int LineNumber;
};

struct TimerResult {

    const char* Name{};
    long long Start{}, End{};
    int ThreadID{};

    TimerMetadata Metadata;

    [[nodiscard]] std::chrono::duration<float> GetDurationMilliseconds() const {
        return std::chrono::milliseconds(End - Start);
    }
    [[nodiscard]] std::chrono::microseconds GetDurationMicroseconds() const {
        return std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::microseconds(End - Start));
    }
};

// Todo: create base class for scoped and managed timer?

struct ScopedProfileTimer {
    const char* mName;
    Profiler* mProfiler;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    TimerMetadata metadata;
    mutable bool HasBeenSampled = false;

    explicit ScopedProfileTimer(const char* aName, Profiler& aBoundProfiler, const char* aFunctionSignature, const int aLineNumber) : metadata(aFunctionSignature, aLineNumber) {
        start = std::chrono::high_resolution_clock::now();
        mName = aName;
        mProfiler = &aBoundProfiler;
    }

    ~ScopedProfileTimer() {
        end = std::chrono::high_resolution_clock::now();
        mProfiler->EndSample(GetResult());
    }

    [[nodiscard]] TimerResult GetResult() const {
        return {
            mName,
            std::chrono::time_point_cast<std::chrono::microseconds>(start).time_since_epoch().count(),
            std::chrono::time_point_cast<std::chrono::microseconds>(end).time_since_epoch().count(),
            0,
            metadata
        };
    }
};
struct ManagedProfileTimer {
    const char* mName;
    Profiler* mProfiler;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    TimerMetadata metadata;
    mutable bool HasBeenSampled = false;

    explicit ManagedProfileTimer(const char* aName, Profiler& aBoundProfiler, const char* aFunctionSignature, const int aLineNumber) : metadata(aFunctionSignature, aLineNumber) {
        start = std::chrono::high_resolution_clock::now();
        mName = aName;
        mProfiler = &aBoundProfiler;
    }

    void StopTimer() {
        end = std::chrono::high_resolution_clock::now();
        mProfiler->EndSample(GetResult());
    }

    [[nodiscard]] TimerResult GetResult() const {
        return {
            mName,
            std::chrono::time_point_cast<std::chrono::microseconds>(start).time_since_epoch().count(),
            std::chrono::time_point_cast<std::chrono::microseconds>(end).time_since_epoch().count(),
            0,
            metadata
        };
    }
};