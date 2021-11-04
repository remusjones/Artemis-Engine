//
// Created by Remus on 4/11/2021.
// Generic Application process to run the "project"
//
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#ifndef INC_3DENGINE_DEFAULTAPPLICATION_H
#define INC_3DENGINE_DEFAULTAPPLICATION_H
#include <vector>

class DefaultApplication
{
public:
    DefaultApplication(const char* windowName, int windowWidth, int windowHeight);
    void Run();



private:
    void InitializeVulkan();
    void InitializeWindow();
    void Update();
    void Cleanup();
    void CreateInstance();


    uint32_t m_extensionCount{};
    int m_windowWidth;
    int m_windowHeight;
    const char* m_windowName;
    VkInstance m_vulkanInstance{};
    GLFWwindow* m_window{};


    std::vector<VkExtensionProperties> m_extensions;




    // =======Validation=====

    bool CheckValidationLayerSupport();

    const std::vector<const char*> m_validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData);
    std::vector<const char*> GetRequiredExtensions();
    void SetupDebugMessenger();
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks* pAllocator,
                                          VkDebugUtilsMessengerEXT* pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

    VkDebugUtilsMessengerEXT m_debugMessenger;
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif
};


#endif //INC_3DENGINE_DEFAULTAPPLICATION_H
