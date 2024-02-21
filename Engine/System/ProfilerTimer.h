#pragma once
#include <chrono>


struct TimerInformation {
    std::string name;
    std::chrono::duration<float> duration{};

    [[nodiscard]] std::chrono::duration<float> GetMilliseconds() const {
        return duration * 1000.0f;
    }

};

struct ProfilerTimer {

    std::chrono::time_point<std::chrono::system_clock> start, end;

    explicit ProfilerTimer(const std::string& aName) {
        start = std::chrono::high_resolution_clock::now();
        timerInformation.name = aName;
    }

    void StopTimer() {
        end = std::chrono::high_resolution_clock::now();
        timerInformation.duration = end - start;
    }

    [[nodiscard]] TimerInformation GetInformation() const {
        return timerInformation;
    }
private:
    TimerInformation timerInformation;
};
