#pragma once
#include <queue>
#include <stack>
#include "ProfilerTimer.h"
#include "Objects/ImGuiLayer.h"

class Profiler : public ImGuiLayer{
public:
    static Profiler& GetInstance() {
        static Profiler instance;
        return instance;
    }

    Profiler(Profiler const&) = delete;
    void operator=(Profiler const&) = delete;

    Profiler() = default;
    ~Profiler();

    void BeginSample(const std::string &aName);
    void EndSample();

    [[nodiscard]] bool IsProfilerEmpty() const;
private:
    void EnsureProfilerLimits(const std::string &aName);

public:
    void OnImGuiRender() override;

private:
    std::stack<ProfilerTimer> mTimerStack;

    const int mMaxHistorySize = 1000;
    std::unordered_map<std::string, std::queue<TimerInformation>> mTimerHistory;
};
