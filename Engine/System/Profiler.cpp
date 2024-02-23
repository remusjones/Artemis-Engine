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
    mTimerHistory[sample.name].emplace_back(sample);
    EnsureProfilerLimits(sample.name);
}

void Profiler::EnsureProfilerLimits(const std::string& aName) {
    auto& history = mTimerHistory[aName];
    size_t size = history.size();
    while (size > mMaxHistorySize) {
        history.pop_front();
        --size;
    }
}

void Profiler::OnImGuiRender() {
    ImGui::Begin("Profiler");
    for (auto& pair : mTimerHistory) {
        std::deque<TimerInformation>& tempDeque = pair.second;
        std::vector<float> durations;
        float sum = 0.0f;
        int count = tempDeque.size();

        for(auto& info : tempDeque) {
            float duration = info.duration.count() * 1000.0f;
            durations.push_back(duration);
            sum += duration;
        }

        float average = count > 0 ? sum / (float)count : 0.0f;
        char overlay[32];
        sprintf(overlay, "%.3f ms", average);
        if (!durations.empty()) {
            ImGui::PlotHistogram(pair.first.c_str(), &durations[0], count, 0, overlay);
        }
    }
    ImGui::End();
}

bool Profiler::IsProfilerEmpty() const {
    return mTimerStack.empty();
}