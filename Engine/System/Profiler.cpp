#include "Profiler.h"

#include "imgui.h"
#include "Logger.h"

#define TRACE 1

Profiler::Profiler() {

#if TRACE
    StartTraceSession();
#endif
}

Profiler::~Profiler() {
    while (!mTimerStack.empty()) {
        Logger::LogError("Profiler: Failed to end sample \"" + std::string(mTimerStack.top().mName) + "\".");
        EndSample();
    }
#if TRACE
    EndTraceSession();
#endif
}

// TODO: nested profiling tracking for sample > sample
// TODO: make name unique?
void Profiler::BeginSample(const char* aName) {
    mTimerStack.emplace(aName);
}

void Profiler::EndSample() {
    ProfilerTimer timer = mTimerStack.top();
    timer.StopTimer();
    mTimerStack.pop();

    auto sample = timer.GetResult();
    mTimerHistory[sample.Name].emplace_back(sample);

#if TRACE
    ExportTraceFrame(sample);
#endif

    EnsureProfilerLimits(sample.Name);
}

void Profiler::EnsureProfilerLimits(const std::string& aName) {
    auto& history = mTimerHistory[aName];
    size_t size = history.size();
    while (size > mMaxDisplayedHistorySize) {
        history.pop_front();
        --size;
    }
}

void Profiler::StartTraceSession() {
    mSessionOutputStream.open(mSessionTraceFilename, std::ios::out);
    mSessionOutputStream << "{\"otherData\": {}, \"traceEvents\":[";
    mSessionOutputStream.flush();
}

void Profiler::ExportTraceFrame(const TimerResult &aResult) {
    std::lock_guard lock(mTraceWriteMutex);

    if (mTraceFrameCounter++ > 0) {
        mSessionOutputStream << ",";
    }

    std::string name = aResult.Name;
    std::replace(name.begin(), name.end(), '"', '\'');

    mSessionOutputStream << "{";
    mSessionOutputStream << "\"name\":\"" << name << "\",";
    mSessionOutputStream << "\"ph\":\"X\",";
    mSessionOutputStream << "\"ts\":" << aResult.Start << ",";
    mSessionOutputStream << "\"dur\":" << aResult.End - aResult.Start << ",";
    mSessionOutputStream << "\"pid\":0,";
    mSessionOutputStream << "\"tid\":" << aResult.ThreadID;
    mSessionOutputStream << "}";
    mSessionOutputStream.flush();
    mTraceWriteMutex.unlock();
}

void Profiler::EndTraceSession() {
    mSessionOutputStream << "]}";
    mSessionOutputStream.flush();
    mSessionOutputStream.close();
}

void Profiler::OnImGuiRender() {
    ImGui::Begin("Profiler");
    for (auto&[fst, snd] : mTimerHistory) {

        std::deque<TimerResult>& tempDeque = snd;
        std::vector<float> durations;

        float sum = 0.0f;
        const int count = tempDeque.size();

        for(auto& info : tempDeque) {
            float duration = info.GetDurationMilliseconds().count();
            durations.push_back(duration);
            sum += duration;
        }

        const float average = count > 0 ? sum / static_cast<float>(count) : 0.0f;
        char overlay[32];
        sprintf(overlay, "%.3f ms", average);
        if (!durations.empty()) {
            ImGui::PlotHistogram(fst.c_str(), &durations[0], count, 0, overlay, 0.0f, 2.0f);
        }
    }
    ImGui::End();
}

bool Profiler::IsProfilerEmpty() const {
    return mTimerStack.empty();
}