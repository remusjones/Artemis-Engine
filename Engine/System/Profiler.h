#pragma once
#include <fstream>
#include <mutex>
#include <stack>
#include "ProfilerTimer.h"
#include "Objects/ImGuiLayer.h"

#ifdef _MSC_VER
    #define FUNCTION_SIGNATURE __FUNCSIG__
#else
    #define FUNCTION_SIGNATURE __PRETTY_FUNCTION__
#endif

//#define PROFILE_FUNCTION(x) Profiler::GetInstance().BeginSample((x + std::string(FUNCTION_SIGNATURE) + std::to_string(##__LINE__).c_str())
#define PROFILE_FUNCTION() Profiler::GetInstance().BeginSample(FUNCTION_SIGNATURE)

class Profiler final : public ImGuiLayer{
public:
    // TODO: Maybe make a profiler factory for different sessions/groups?
    static Profiler& GetInstance() {
        static Profiler instance;
        return instance;
    }

    Profiler(Profiler const&) = delete;
    void operator=(Profiler const&) = delete;

    Profiler();
    ~Profiler() override;

    void BeginSample(const char *aName);
    void EndSample();

    [[nodiscard]] bool IsProfilerEmpty() const;
private:
    void EnsureProfilerLimits(const std::string &aName);
    void StartTraceSession();
    void ExportTraceFrame(const TimerResult& aResult);
    void EndTraceSession();

public:
    void OnImGuiRender() override;

private:
    std::stack<ProfilerTimer> mTimerStack;
    std::unordered_map<std::string,std::deque<TimerResult>> mTimerHistory;
    const int mMaxDisplayedHistorySize = 1000;
    const char* mSessionTraceFilename = "trace.json";
    std::mutex mTraceWriteMutex;
    std::ofstream mSessionOutputStream;
    long long mTraceFrameCounter = 0;
};
