#pragma once
#include <queue>
#include <stack>
#include "ProfilerTimer.h"
#include "Objects/ImGuiLayer.h"

class Profiler : public ImGuiLayer{
public:

    Profiler() = default;
    ~Profiler();

    void BeginSample(const std::string &aName);
    void EndSample();

    [[nodiscard]] bool IsProfilerEmpty() const;
private:
    void EnsureProfilerLimits();

public:
    void OnImGuiRender() override;

private:
    std::stack<ProfilerTimer> mTimerStack;

    const int mMaxHistorySize = 1000;
    std::queue<TimerInformation> mTimerHistory;
};
