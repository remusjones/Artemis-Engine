//
// Created by Remus on 29/12/2023.
//

#pragma once
#include <functional>
#include <SDL_events.h>
#include <unordered_map>

struct KeyboardEvent {
    KeyboardEvent();

    KeyboardEvent(const bool aPressed, const SDL_Keymod aKeyState): mPressedState(aPressed), mModState(aKeyState) {
    }

    bool mPressedState;
    SDL_Keymod mModState;
};

struct InputBinding {
    InputBinding(const char *aBindingName,
                 std::function<void(KeyboardEvent aKeyboardEvent)> &&aCallback) : mBindingName(aBindingName),
        mCallback(aCallback) {
    };

    const char *mBindingName;
    std::function<void(KeyboardEvent aKeyboardEvent)> mCallback;
};

// TODO: Add Input event map bindings
class InputManager {
public:
    InputManager();

    void ProcessInput(const SDL_Event *aInputEvent);

    void RegisterKeyCodeInput(
        SDL_KeyCode aKeyCode,
        std::function<void(KeyboardEvent aKeyboardEvent)> &&aCallback,
        const char *aBindingName = "Default"
    );

    std::unordered_map<int32_t, std::vector<InputBinding> > mKeyboardBindings;
};



extern InputManager *gInputManager;
