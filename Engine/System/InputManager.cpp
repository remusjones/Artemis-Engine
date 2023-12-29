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

           default: break;
       }
   }
}

void InputManager::RegisterKeyCodeInput(SDL_KeyCode aKeyCode,
                                        std::function<void(KeyboardEvent aKeyboardEvent)> &&aCallback,
                                        const char *aBindingName) {
    InputBinding newBinding(aBindingName, (std::move(aCallback)));
    mKeyboardBindings[aKeyCode].push_back(newBinding);
}
