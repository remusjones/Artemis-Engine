//
// Created by Remus on 21/02/2024.
//

#include "Profiler.h"

#include "Logger.h"

Profiler::~Profiler() {
    while (!mTimerStack.empty()) {
        Logger::LogError("Profiler: Failed to end sample \"" + mTimerStack.top().GetInformation().name + "\".");
        EndSample();
    }
}

void Profiler::BeginSample(const std::string &aName) {
    mTimerStack.emplace(aName);
}
void Profiler::EndSample() {
    ProfilerTimer timer = mTimerStack.top();
    timer.StopTimer();
    mTimerStack.pop();

    mTimerHistory.emplace(timer.GetInformation());
    EnsureProfilerLimits();
}

void Profiler::EnsureProfilerLimits() {
    if (mTimerHistory.size() > mMaxHistorySize) {
        mTimerHistory.pop();
    }
}

void Profiler::OnImGuiRender() {
    // TODO: Implement Profiler::OnImGuiRender with Graphing and History
}

bool Profiler::IsProfilerEmpty() const {
    return mTimerStack.empty();
}

