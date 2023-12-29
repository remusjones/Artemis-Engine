//
// Created by Remus on 29/12/2023.
//

#include "InputManager.h"
#include "glog/logging.h"

InputManager *gInputManager = nullptr;

InputManager::InputManager() {
    gInputManager = this;

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
