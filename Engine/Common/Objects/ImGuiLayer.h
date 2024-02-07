//
// Created by Remus on 24/12/2023.
//

#pragma once
#include <string>
#include <unordered_map>

class ImGuiLayer {
public:
    ImGuiLayer();
    virtual void OnImGuiRender() = 0;
    const char* GetUniqueLabel(const char* aLabel);


private:

    /**
     * \brief
     * Not Guaranteed to be Unique, but good enough for this use-case for now
     * https://stackoverflow.com/questions/24365331/how-can-i-generate-uuid-in-c-without-using-boost-library
     * \return
     * Unique String
     */
    static std::string MakeGuid();
    std::unordered_map<const char*, std::string> mConstructedLabels;
};
