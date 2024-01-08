//
// Created by Remus on 4/11/2021.
// Generic VulkanGraphicsImpl process to run the "project"
//
#pragma once
#include <functional>
#include <memory>
#include <vector>
#include <SDL_video.h>

#include "DequeBuffer.h"
#include "IApplication.h"
#include "InputManager.h"
#include "Vulkan/VulkanEngine.h"
#include "Vulkan/VulkanSystemStructs.h"

class Scene;
class VulkanSwapChain;
class MeshObject;

// TODO: This should really only contain window information, and initial vulkan creation. Everything else
// TODO: should probably go within the Engine class
class VulkanGraphicsImpl : public IApplication {
public:
    void Run() override;

    VulkanGraphicsImpl(const char *aWindowName, int aWindowWidth, int aWindowHeight);

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &aAvailableFormats);

    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &aAvailablePresentModes);

    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &aCapabilities) const;

    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice aDevice) const;

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice aDevice) const;

    QueueFamilyIndices GetQueueFamilyIndices() const { return mFamilyIndices; }
    float DeltaTimeUnscaled() const { return mDeltaTime; }
    float GetFps() const { return mFps; }

    DequeBuffer &GetFpsHistory() {return mFPSCircularBuffer;}

private:
    // Init Vulkan & Supporting Vulkan
    void InitializeVulkan();

    void InitializeImgui();

    void ShutdownImgui() const;

    // void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);
    void ShutdownVulkan() const;

    // Create Window
    void InitializeWindow();

    void ShutdownWindow() const;


    // Main Engine Loop
    void Update();

    // Deconstructor
    void Cleanup();

    // Create Instance of Vulkan
    void CreateInstance();

    void DestroyInstance() const;

    // Setup Debug Messenger for debug conditions
    void CreateDebugMessenger();

    void DestroyDebugMessenger();

    // specify queues to be created & features for engine
    void CreateLogicalDevice();

    void DestroyLogicalDevice();

    // Create WSI > Vulkan bridge
    void CreateSurface();

    void DestroySurface() const;

    void CreateGraphicsPipeline();

    void DestroyGraphicsPipeline();

    void CreateScenes();

    void DestroyScenes() const;

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT aMessageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT aMessageType,
        const VkDebugUtilsMessengerCallbackDataEXT *aCallbackData,
        void *aUserData);

    std::vector<const char *> GetRequiredExtensions() const;

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *aCreateInfo,
                                          const VkAllocationCallbacks *aAllocator,
                                          VkDebugUtilsMessengerEXT *aDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(VkInstance aInstance, VkDebugUtilsMessengerEXT aDebugMessenger,
                                       const VkAllocationCallbacks *aAllocator);

    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &aCreateInfo);

    bool CheckValidationLayerSupport() const;

public:
    VulkanEngine mVulkanEngine;
    VulkanSwapChain *mSwapChain;
    VkInstance mVulkanInstance;
    SDL_Window *mWindow{nullptr}; // TODO: Move to interface
    VkDevice mLogicalDevice{};
    VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
    VkSurfaceKHR mSurface;
    VmaAllocator mAllocator;
    InputManager mInputManager;
    std::unique_ptr<Scene> mActiveScene;

private:
    QueueFamilyIndices mFamilyIndices;
    // TODO: Move these to IApplication
    int mWindowWidth;
    int mWindowHeight;
    const char *mWindowName;
    float mDeltaTime;
    float mFps;
    VkDescriptorPool mImguiPool;

    DequeBuffer mFPSCircularBuffer;
    // Vulkan Impls
    std::vector<VkExtensionProperties> mExtensions;
    VkQueue mGraphicsQueue;
    VkQueue mPresentQueue;
    VkRenderPass mRenderPass{};
    const std::vector<const char *> m_deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    void InitializePhysicalDevice();

    bool IsDeviceSuitable(VkPhysicalDevice aPhysicalDevice);

    bool CheckDeviceExtensionSupport(VkPhysicalDevice aPhysicalDevice) const;


    VkPhysicalDeviceFeatures mDeviceFeatures{};

    const std::vector<const char *> mValidationLayers = {
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
extern VulkanGraphics *gGraphics;
