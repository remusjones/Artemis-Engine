#include "Profiler.h"

#include "imgui.h"
#include "Logger.h"

Profiler::~Profiler() {
    while (!mTimerStack.empty()) {
        Logger::LogError("Profiler: Failed to end sample \"" + mTimerStack.top().GetInformation().name + "\".");
        EndSample();
    }
}

// TODO: nested profiling tracking for sample > sample
// TODO: make name unique?
void Profiler::BeginSample(const std::string &aName) {
    mTimerStack.emplace(aName);
}

void Profiler::EndSample() {
    ProfilerTimer timer = mTimerStack.top();
    timer.StopTimer();
    mTimerStack.pop();

    auto sample = timer.GetInformation();
    mTimerHistory[sample.name].push(sample);
    EnsureProfilerLimits(sample.name);
}

void Profiler::EnsureProfilerLimits(const std::string& aName) {
    auto& history = mTimerHistory[aName];
    size_t size = history.size();
    while (size > mMaxHistorySize) {
        history.pop();
        --size;
    }
}

void Profiler::OnImGuiRender() {
    ImGui::Begin("Profiler");
    for (auto& pair : mTimerHistory) {
        std::vector<float> durations;
        std::queue<TimerInformation> tempQueue = pair.second;
        float sum = 0.0f;
        int count = tempQueue.size();

        while (!tempQueue.empty()) {
            float duration = tempQueue.front().duration.count() * 1000.0f;
            durations.push_back(duration);
            sum += duration;
            tempQueue.pop();
        }

        float average = (count > 0) ? sum / count : 0.0f;
        char overlay[32];
        sprintf(overlay, "%.3f ms", average);
        if (!durations.empty()) {
            ImGui::PlotHistogram(pair.first.c_str(), &durations[0], durations.size(),0.f, overlay);
        }
    }
    ImGui::End();
}

bool Profiler::IsProfilerEmpty() const {
    return mTimerStack.empty();
}