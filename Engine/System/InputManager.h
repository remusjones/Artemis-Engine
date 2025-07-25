#pragma once
#include <functional>
#include <queue>
#include <SDL_events.h>
#include <unordered_map>

#include "Base/Common/Buffers/Texture.h"


struct KeyboardEvent {
    KeyboardEvent();

    KeyboardEvent(const bool aPressed, const SDL_Keymod aKeyState): mPressedState(aPressed), mModState(aKeyState) {
    }

    bool mPressedState;
    SDL_Keymod mModState;


};

struct KeyCodeInputBinding {
    KeyCodeInputBinding(const char *aBindingName,
                        std::function<void(KeyboardEvent aKeyboardEvent)> &&aCallback) : mBindingName(aBindingName),
        mCallback(aCallback) {
    };

    const char *mBindingName;
    std::function<void(KeyboardEvent aKeyboardEvent)> mCallback;
};

struct MouseMotionBinding {
    MouseMotionBinding(const char *aBindingName,
                       std::function<void(SDL_MouseMotionEvent aMouseMotionEvent)> &&aCallback) : mBindingName(
            aBindingName),
        mCallback(aCallback) {
    };

    const char *mBindingName;
    std::function<void(SDL_MouseMotionEvent)> mCallback;
};

struct MouseInputBinding {
    MouseInputBinding(const char *aBindingName,
                       std::function<void(SDL_MouseButtonEvent aMouseButtonEvent)> &&aCallback) : mBindingName(
            aBindingName),
        mCallback(aCallback) {
    };

    const char *mBindingName;
    std::function<void(SDL_MouseButtonEvent)> mCallback;
};

// TODO: Add Input event map bindings
class InputManager {
public:
    InputManager();

    void ConsumeInput(const SDL_Event *aInputEvent);
    void ProcessInput(const SDL_Event *aInputEvent);
    void Update();

    void RegisterKeyCodeInput(
        SDL_KeyCode aKeyCode,
        std::function<void(KeyboardEvent aKeyboardEvent)> &&aCallback,
        const char *aBindingName = "Default"
    );

    void RegisterMouseInput(
        std::function<void(SDL_MouseMotionEvent)> &&aCallback,
        const char *aBindingName = "Default");

    void RegisterMouseInput(
        std::function<void(SDL_MouseButtonEvent)> &&aCallback,
        const char *aBindingName = "Default");

    // TODO: make descriptor map
    std::unordered_map<int32_t, std::vector<KeyCodeInputBinding>> mKeyboardBindings;

    // TODO: make map
    std::vector<MouseMotionBinding> mMouseMotionBindings;
    std::vector<MouseInputBinding> mMouseInputBindings;

    std::queue<SDL_Event> mQueuedEvents;

};

extern InputManager *gInputManager;
