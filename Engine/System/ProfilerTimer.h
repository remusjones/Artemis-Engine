#pragma once
#include <chrono>


struct TimerResult {
    const char* Name;
    long long Start, End;

    [[nodiscard]] std::chrono::duration<float> GetDurationMilliseconds() const {
        return std::chrono::milliseconds(End - Start);
    }
    [[nodiscard]] std::chrono::microseconds GetDurationMicroseconds() const {
        return std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::microseconds(End - Start));
    }
};

struct ProfilerTimer {

    const char* mName;
    std::chrono::time_point<std::chrono::system_clock> start, end;


    explicit ProfilerTimer(const char* aName) {
        start = std::chrono::high_resolution_clock::now();
        mName = aName;
    }

    void StopTimer() {
        end = std::chrono::high_resolution_clock::now();
    }

    [[nodiscard]] TimerResult GetResult() const {
        return {
            mName,
            std::chrono::time_point_cast<std::chrono::microseconds>(start).time_since_epoch().count(),
            std::chrono::time_point_cast<std::chrono::microseconds>(end).time_since_epoch().count()
        };
    }
};
