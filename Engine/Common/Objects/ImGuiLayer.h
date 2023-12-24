//
// Created by Remus on 24/12/2023.
//

#pragma once

class ImGuiLayer {
public:
    virtual void OnImGuiRender() = 0; // TODO: Invoke from single point in app
};
