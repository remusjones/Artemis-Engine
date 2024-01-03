#pragma once
#include <queue>

class DequeBuffer {
public:
    DequeBuffer(const size_t size)
        : mSize(size), mCurrentIndex(0) {
    }

    void AddElement(float value) {
        mBuffer.emplace_back(value);
        if (mBuffer.size() > mSize)
            mBuffer.pop_front();
    }

    std::deque<float> mBuffer;
    size_t mSize;
    size_t mCurrentIndex;
};