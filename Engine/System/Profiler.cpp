//
// Created by Remus on 21/02/2024.
//

#include "Profiler.h"

#include "imgui.h"
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
    ImGui::Begin("Profiler");

    // TODO: Sort by name(s) and duration(s)
    std::vector<float> durations;
    std::queue<TimerInformation> tempQueue = mTimerHistory;
    while (!tempQueue.empty()) {
        durations.push_back(tempQueue.front().duration.count() * 1000.0f);
        tempQueue.pop();
    }

    if (!durations.empty()) {
        ImGui::PlotHistogram("Timer Durations", &durations[0], durations.size());
    }

    ImGui::End();
}

bool Profiler::IsProfilerEmpty() const {
    return mTimerStack.empty();
}

