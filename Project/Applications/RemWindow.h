//
// Created by Remus on 4/11/2021.
// Generic Application process to run the "project"
//
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#ifndef INC_3DENGINE_DEFAULTAPPLICATION_H
#define INC_3DENGINE_DEFAULTAPPLICATION_H
#include <vector>
#include <optional>
#include "RemPipeline.h"
#include "SystemStructs.h"


class RemWindow
{
public:
    RemWindow(const char* windowName, int windowWidth, int windowHeight);
    void Run();



private:

    // Init Vulkan & Supporting Systems
    void InitializeVulkan();

    // Create GLFW Window
    void InitializeWindow();

    // Main Engine Loop
    void Update();

    // Deconstructor
    void Cleanup();

    // Create Instance of Vulkan SDK
    void CreateInstance();

    // Setup Debug Messenger for debug conditions
    void SetupDebugMessenger();

    // specify queues to be created & features for engine
    void CreateLogicalDevice();

    // Create WSI > Vulkan bridge
    void CreateSurface();
    void RecreateSwapChain();
    void CreateSwapChain();
    void CreateImageViews();
    void CreateGraphicsPipeline();
    void CreateFrameBuffers();

    VkSurfaceFormatKHR  ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);


    // Window Variables
    GLFWwindow* m_window{};
    int m_windowWidth;
    int m_windowHeight;
    const char* m_windowName;

    // Vulkan Variables
    VkInstance m_vulkanInstance;
    uint32_t m_extensionCount{};
    std::vector<VkExtensionProperties> m_extensions;
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
    VkSurfaceKHR m_surface;

    RemPipeline m_renderPipeline{};



    // swapchain
    VkSwapchainKHR m_swapChain;
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;
    std::vector<VkImage> m_swapChainImages;
    std::vector<VkImageView> m_swapChainImageViews;

    const std::vector<const char*> m_deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };



    // =======Hardware=====
    void InitializePhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice physicalDevice);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    QueueFamilyIndices  FindQueueFamilies(VkPhysicalDevice device);
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

    // Reference to our target device
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    // our logical device handle to interface
    VkDevice  m_logicalDevice{};
    VkPhysicalDeviceFeatures m_deviceFeatures{};



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

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks* pAllocator,
                                          VkDebugUtilsMessengerEXT* pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    VkDebugUtilsMessengerEXT m_debugMessenger;
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

};

#endif
