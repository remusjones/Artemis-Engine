//
// Created by Remus on 4/11/2021.
// Generic VulkanGraphicsImpl process to run the "project"
//
#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <optional>
#include "VulkanPipelineManager.h"
#include "VulkanSystemStructs.h"
#include "IApplication.h"

class VulkanSwapChain;
class SquareObject;
class VulkanGraphicsImpl : public IApplication
{
public:
    void Run() override;

    VulkanGraphicsImpl(const char* aWindowName, int aWindowWidth, int aWindowHeight);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& aAvailableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& aAvailablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& aCapabilities);
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice aDevice);
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice aDevice);

private:

    // Init Vulkan & Supporting Vulkan
    void InitializeVulkan();

    // Create GLFW Window
    void InitializeWindow();

    // Main Engine Loop
    void Update();

    // Deconstructor
    void Cleanup();

    // Create Instance of Vulkan
    void CreateInstance();

    // Setup Debug Messenger for debug conditions
    void SetupDebugMessenger();

    // specify queues to be created & features for engine
    void CreateLogicalDevice();

    // Create WSI > Vulkan bridge
    void CreateSurface();

    void CreateGraphicsPipeline();
    void CreateObjects();

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT aMessageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT aMessageType,
            const VkDebugUtilsMessengerCallbackDataEXT* aCallbackData,
            void* aUserData);

    std::vector<const char*> GetRequiredExtensions() const;

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* aCreateInfo,
                                          const VkAllocationCallbacks* aAllocator,
                                          VkDebugUtilsMessengerEXT* aDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(VkInstance aInstance, VkDebugUtilsMessengerEXT aDebugMessenger, const VkAllocationCallbacks* aAllocator);
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& aCreateInfo);
    bool CheckValidationLayerSupport();

public:
    // TODO Move to HEAP
    VulkanPipelineManager mRenderPipelineManager;
    VulkanSwapChain* mSwapChain;
    VkInstance mVulkanInstance;
    GLFWwindow* mWindow{}; // TODO: Move to interface
    VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
    VkDevice  mLogicalDevice{};

private:

    // TODO: Move these to IApplication
    int mWindowWidth;
    int mWindowHeight;
    const char* mWindowName;

    SquareObject* mTriangle;

    // Vulkan Impls
    std::vector<VkExtensionProperties> mExtensions;
    VkQueue mGraphicsQueue;
    VkQueue mPresentQueue;
    VkSurfaceKHR mSurface;
    VkRenderPass mRenderPass{};
    const std::vector<const char*> m_deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    void InitializePhysicalDevice();
    bool IsDeviceSuitable(VkPhysicalDevice aPhysicalDevice);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice aPhysicalDevice);


    VkPhysicalDeviceFeatures mDeviceFeatures{};

    const std::vector<const char*> mValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

    VkDebugUtilsMessengerEXT mDebugMessenger;

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

};
typedef VulkanGraphicsImpl VulkanGraphics;
extern VulkanGraphics* gGraphics;