#include "InputManager.h"
#include <imgui_impl_sdl3.h>
#include <iostream>

InputManager *gInputManager = nullptr;

InputManager::InputManager() {
    gInputManager = this;
}

void InputManager::ConsumeInput(const SDL_Event *aInputEvent) {
    ImGui_ImplSDL3_ProcessEvent(aInputEvent);
    if (aInputEvent->key.repeat == 0)
        mQueuedEvents.push(*aInputEvent);
}

void InputManager::ProcessInput(const SDL_Event *aInputEvent) {
    if (aInputEvent->key.repeat == 0) {
        switch (aInputEvent->type) {
            case SDL_EVENT_KEY_DOWN:
                for (const auto &binding: mKeyboardBindings[aInputEvent->key.keysym.sym]) {
                    const KeyboardEvent kbEvent(aInputEvent->key.state, SDL_GetModState());
                    binding.mCallback(kbEvent);
                }
            case SDL_EVENT_KEY_UP:
                for (const auto &binding: mKeyboardBindings[aInputEvent->key.keysym.sym]) {
                    const KeyboardEvent kbEvent(aInputEvent->key.state, SDL_GetModState());
                    binding.mCallback(kbEvent);
                }
            case SDL_EVENT_MOUSE_MOTION:
                for (const auto &binding: mMouseMotionBindings) {
                    binding.mCallback(aInputEvent->motion);
                }

            // Mouse events getting sent regardless of input changed state
            case SDL_EVENT_MOUSE_BUTTON_UP:
                for (const auto &binding: mMouseInputBindings) {
                    binding.mCallback(aInputEvent->button);
                }
            break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                for (const auto &binding: mMouseInputBindings) {
                    binding.mCallback(aInputEvent->button);
                }
            break;
            default: break;
        }
    }
}

// TODO: Propery filter out duplicate events, and queue only uniques so we can consume during Update
void InputManager::Update() {
    while (!mQueuedEvents.empty()) {
        ProcessInput(&mQueuedEvents.front());
        mQueuedEvents.pop();
    }
}

void InputManager::RegisterMouseInput(std::function<void(SDL_MouseMotionEvent)> &&aCallback,
                                      const char *aBindingName) {
    const MouseMotionBinding newBinding(aBindingName, std::move(aCallback));
    mMouseMotionBindings.push_back(newBinding);
}

void InputManager::RegisterMouseInput(std::function<void(SDL_MouseButtonEvent)> &&aCallback, const char *aBindingName) {
    const MouseInputBinding newBinding(aBindingName, std::move(aCallback));
    mMouseInputBindings.push_back(newBinding);
}

void InputManager::RegisterKeyCodeInput(SDL_KeyCode aKeyCode,
                                        std::function<void(KeyboardEvent aKeyboardEvent)> &&aCallback,
                                        const char *aBindingName) {
    const KeyCodeInputBinding newBinding(aBindingName, (std::move(aCallback)));
    mKeyboardBindings[aKeyCode].push_back(newBinding);
}
