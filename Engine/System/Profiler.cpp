#include "Profiler.h"

#include "imgui.h"
#include "Logger.h"
#include "ScopedProfileTimer.h"

#define TRACE 1 // Move to state or config

Profiler::Profiler() {
#if TRACE
    StartTraceSession();
#endif
}

Profiler::~Profiler() {
#if TRACE
    EndTraceSession();
#endif

    while (!mTimerStack.empty()) {
        const auto item = mTimerStack.top();
        Logger::LogError(std::string("Profiler: Timer stack not empty at destruction ") + item.mName);
        mTimerStack.pop();
    }
}

void Profiler::EndSample(const TimerResult &aResult) {
    mTimerHistory[aResult.Name].emplace_back(aResult);

#if TRACE
    ExportTraceFrame(aResult);
#endif
    EnsureProfilerLimits(aResult.Name);
}

void Profiler::BeginProfile(const char* aName, const char* aFunctionSignature, const int aLineNumber) {
    if (!mRunning)
        return;

    mTimerStack.emplace(aName, *this, aFunctionSignature, aLineNumber);
}

void Profiler::EndProfile() {
    if (!mRunning || mTimerStack.empty()) // Catch incase of mismatched begin/end
        return;

    mTimerStack.top().StopTimer();
    mTimerStack.pop();
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
// TODO: Convert to ProtoBuf format
void Profiler::ExportTraceFrame(const TimerResult &aResult) {
    std::lock_guard lock(mTraceWriteMutex);

    if (mTraceFrameCounter++ > 0) {
        mSessionOutputStream << ",";
    }

    std::string name = aResult.Name;
    std::replace(name.begin(), name.end(), '"', '\'');

    if (!name.empty()) {
        name.append("_");
    }
    name.append(std::string(aResult.Metadata.FunctionSignature) + ":"+ std::to_string(aResult.Metadata.LineNumber));

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
    if (ImGui::Checkbox("Running", &mRunning)) {
        if (!mRunning) {
            while(!mTimerStack.empty()) {
                mTimerStack.top().StopTimer();
                mTimerStack.pop();
            }
            EndTraceSession();
        } else {
            StartTraceSession();
        }
    }

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