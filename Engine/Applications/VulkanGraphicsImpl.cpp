//
// Created by Remus on 4/11/2021.
//
#define VMA_IMPLEMENTATION
#define VK_USE_PLATFORM_WIN32_KHR
#include "VulkanGraphicsImpl.h"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <set>
#include <stdexcept>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>

#include "imgui.h"
#include "InputManager.h"
#include "Logger.h"
#include "ImGuizmo.h"
#include "Scenes/SandboxScene.h"
#include "Vulkan/Common/MeshObject.h"

VulkanGraphics* gGraphics = nullptr;

void VulkanGraphicsImpl::Run()
{
    InitializeWindow();
    InitializeVulkan();
    Update();
    Cleanup();
}

void VulkanGraphicsImpl::InitializeVulkan()
{
    gGraphics = this;
    CreateInstance();
    CreateDebugMessenger();
    CreateSurface();
    InitializePhysicalDevice();
    CreateLogicalDevice();


    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = mPhysicalDevice;
    allocatorInfo.device = mLogicalDevice;
    allocatorInfo.instance = mVulkanInstance;
    vmaCreateAllocator(&allocatorInfo, &mAllocator);

    mSwapChain = new VulkanSwapChain();
    mSwapChain->Initialize(mLogicalDevice,
                           mPhysicalDevice,
                           mSurface,
                           mRenderPass,
                           this);
    CreateGraphicsPipeline();
    InitializeImgui();
}

void VulkanGraphicsImpl::InitializeImgui()
{
    //1: create descriptor pool for IMGUI
    // the size of the pool is very oversize, but it's copied from imgui demo itself.
    VkDescriptorPoolSize pool_sizes[] =
    {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    vkCreateDescriptorPool(mLogicalDevice, &pool_info, nullptr, &mImguiPool);


    ImGui::CreateContext();

    ImGui_ImplSDL3_InitForVulkan(mWindow);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = mVulkanInstance;
    init_info.PhysicalDevice = mPhysicalDevice;
    init_info.Device = mLogicalDevice;
    init_info.Queue = mGraphicsQueue;
    init_info.DescriptorPool = mImguiPool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info, mVulkanEngine.mSwapChain->mRenderPass);

    ImGui_ImplVulkan_CreateFontsTexture();
}

void VulkanGraphicsImpl::ShutdownImgui() const
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    vkDestroyDescriptorPool(mLogicalDevice, mImguiPool, nullptr);
}

void VulkanGraphicsImpl::ShutdownVulkan() const
{
    vmaDestroyAllocator(mAllocator);
}

void VulkanGraphicsImpl::Update()
{
    // Start Clock for FPS Monitoring
    auto startTime = std::chrono::high_resolution_clock::now();
    auto fpsStartTime = std::chrono::high_resolution_clock::now();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // Create FPS Window Header

    const std::string fpsHeader = mWindowName + std::string(" | FPS: ");

    int frameCount = 0;
    SDL_Event e;
    bool bQuitting = false;


    //main loop
    while (!bQuitting)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            gInputManager->ConsumeInput(&e);
            if (e.type == SDL_EVENT_QUIT) bQuitting = true;
        }
        if (!(SDL_GetWindowFlags(mWindow) & SDL_WINDOW_MINIMIZED))
        {
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();
            ImGuizmo::BeginFrame();
            ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
            gInputManager->Update();
            this->mActiveScene->Tick(mDeltaTime);
            this->mVulkanEngine.DrawFrame(*mActiveScene);

            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        // Calculate Frames-Per-Second
        frameCount++;
        auto currentTime = std::chrono::high_resolution_clock::now();
        mDeltaTime = std::chrono::duration_cast<std::chrono::duration<float>>
            (currentTime - startTime).count();

        if (const auto elapsedTime = std::chrono::duration_cast<
                std::chrono::seconds>(currentTime - fpsStartTime).count();
            elapsedTime > 0)
        {
            mFps = frameCount / elapsedTime;
            mFPSCircularBuffer.AddElement(mFps);
            frameCount = -1;
            fpsStartTime = currentTime;
        }
        startTime = currentTime;
    }
}

void VulkanGraphicsImpl::Cleanup()
{
    vkDeviceWaitIdle(mLogicalDevice);
    DestroyScenes();
    ShutdownImgui();
    DestroyGraphicsPipeline();
    ShutdownVulkan();
    DestroyLogicalDevice();
    DestroyDebugMessenger();
    DestroySurface();
    DestroyInstance();

    Logger::Log(spdlog::level::info, "Destroying VulkanGraphicsImpl");
    ShutdownWindow();
}

VulkanGraphicsImpl::VulkanGraphicsImpl(const char* aWindowName,
                                       const int aWindowWidth,
                                       const int aWindowHeight):
    mFPSCircularBuffer(100)
{
    mWindowName = aWindowName;
    mWindowWidth = aWindowWidth;
    mWindowHeight = aWindowHeight;
}

static void WindowResizedCallback(void* userdata, SDL_Event* event)
{
    if (event->window.type == SDL_EVENT_WINDOW_RESIZED)
    {
        gGraphics->mVulkanEngine.QueueFrameBufferRebuild();
    }
}

void VulkanGraphicsImpl::InitializeWindow()
{
    Logger::Log(spdlog::level::debug, "Creating Window");
    // We initialize SDL and create a window with it.
    SDL_Init(SDL_INIT_VIDEO);

    const int windowFlags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE;

    //create blank SDL window for our application
    mWindow = SDL_CreateWindow(
        mWindowName, //window title
        mWindowWidth, //window width in pixels
        mWindowHeight, //window height in pixels
        windowFlags
    );
    SDL_AddEventWatch(reinterpret_cast<SDL_EventFilter>(WindowResizedCallback),
                      nullptr);
}

void VulkanGraphicsImpl::ShutdownWindow() const
{
    if (mWindow != nullptr)
        SDL_DestroyWindow(mWindow);
}

void VulkanGraphicsImpl::CreateInstance()
{
    Logger::Log(spdlog::level::debug, "Creating Vulkan Instance");

    if (enableValidationLayers && !CheckValidationLayerSupport())
    {
        throw std::runtime_error(
            "validation layers requested, but not available!");
    }
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = mWindowName;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;


    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.
            size());
        createInfo.ppEnabledLayerNames = mValidationLayers.data();

        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&
            debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    const auto extensions = GetRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    if (vkCreateInstance(&createInfo, nullptr, &mVulkanInstance) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }
}

void VulkanGraphicsImpl::DestroyInstance() const
{
    vkDestroyInstance(mVulkanInstance, nullptr);
}

void VulkanGraphicsImpl::CreateSurface()
{
    if (SDL_Vulkan_CreateSurface(mWindow, mVulkanInstance, nullptr, &mSurface)
        != SDL_TRUE)
    {
        throw std::runtime_error("failed to create window surface");
    }
}

void VulkanGraphicsImpl::DestroySurface() const
{
    vkDestroySurfaceKHR(mVulkanInstance, mSurface, nullptr);
}

void VulkanGraphicsImpl::CreateScenes()
{
    mActiveScene = new SandboxScene();
    mActiveScene->Construct("Sandbox");
}

void VulkanGraphicsImpl::DestroyScenes() const
{
    mActiveScene->Cleanup();
    delete mActiveScene;
}

void VulkanGraphicsImpl::CreateGraphicsPipeline()
{
    mVulkanEngine.Initialize(mLogicalDevice,
                             mSwapChain,
                             mPhysicalDevice,
                             mGraphicsQueue,
                             mPresentQueue
    );

    mVulkanEngine.CreateSyncObjects();
    mSwapChain->CreateFrameBuffers();
    CreateScenes();
}

void VulkanGraphicsImpl::DestroyGraphicsPipeline()
{
    mVulkanEngine.Cleanup();
    mSwapChain->Cleanup();
    delete mSwapChain;
}

bool VulkanGraphicsImpl::CheckValidationLayerSupport() const
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : mValidationLayers)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

std::vector<const char*> VulkanGraphicsImpl::GetRequiredExtensions() const
{
    Uint32 count = 0;
    const auto windowExtensions = SDL_Vulkan_GetInstanceExtensions(&count);

    std::vector extensions(windowExtensions, windowExtensions + count);

    if (enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }

    return extensions;
}

void VulkanGraphicsImpl::PopulateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT& aCreateInfo)
{
    aCreateInfo = {};
    aCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    aCreateInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    aCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    aCreateInfo.pfnUserCallback = DebugCallback;
}

void VulkanGraphicsImpl::CreateDebugMessenger()
{
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    PopulateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(mVulkanInstance, &createInfo, nullptr,
                                     &mDebugMessenger) != VK_SUCCESS)
    {
        Logger::Log(spdlog::level::err, "Failed to setup debug messenger");
    }
}

void VulkanGraphicsImpl::DestroyDebugMessenger()
{
    if (enableValidationLayers)
    {
        DestroyDebugUtilsMessengerEXT(mVulkanInstance, mDebugMessenger, nullptr);
    }
}

VkResult VulkanGraphicsImpl::CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                          const VkDebugUtilsMessengerCreateInfoEXT* aCreateInfo,
                                                          const VkAllocationCallbacks* aAllocator,
                                                          VkDebugUtilsMessengerEXT* aDebugMessenger)
{
    PFN_vkCreateDebugUtilsMessengerEXT func =
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance,
                                  "vkCreateDebugUtilsMessengerEXT")
        );

    if (func != nullptr)
    {
        return func(instance, aCreateInfo, aAllocator, aDebugMessenger);
    }
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void VulkanGraphicsImpl::DestroyDebugUtilsMessengerEXT(
    VkInstance aInstance, VkDebugUtilsMessengerEXT aDebugMessenger,
    const VkAllocationCallbacks* aAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        aInstance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(aInstance, aDebugMessenger, aAllocator);
    }
}

VkBool32 VulkanGraphicsImpl::DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT aMessageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT aMessageType,
    const VkDebugUtilsMessengerCallbackDataEXT* aCallbackData,
    void* aUserData)
{
    if (aMessageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        //LOG(ERROR) << aMessageType << " | Validation layer: " <<
        //    aCallbackData->pMessage;
        Logger::Log(spdlog::level::err,
                    (std::string("Validation Layer ") +
                        std::string(aCallbackData->pMessage)).c_str());
    }
    return VK_FALSE;
}

void VulkanGraphicsImpl::InitializePhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(mVulkanInstance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(mVulkanInstance, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        if (IsDeviceSuitable(device))
        {
            mPhysicalDevice = device;
            break;
        }
    }

    if (mPhysicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU");
    }
    else
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(mPhysicalDevice, &deviceProperties);
        vkGetPhysicalDeviceFeatures(mPhysicalDevice, &mDeviceFeatures);

        Logger::Log(spdlog::level::debug, (std::string("Selecting GPU: ") + deviceProperties.deviceName).c_str());
    }
}

bool VulkanGraphicsImpl::IsDeviceSuitable(VkPhysicalDevice aPhysicalDevice)
{
    const QueueFamilyIndices indices = FindQueueFamilies(aPhysicalDevice);

    const bool extensionsSupported = CheckDeviceExtensionSupport(aPhysicalDevice);

    bool swapChainAdequate = false;

    if (extensionsSupported)
    {
        const SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(
            aPhysicalDevice);
        swapChainAdequate = !swapChainSupport.mFormats.empty() && !
            swapChainSupport.mPresentModes.empty();
    }

    return indices.IsComplete() && extensionsSupported && swapChainAdequate;
}

VkSurfaceFormatKHR VulkanGraphicsImpl::ChooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat
            .colorSpace ==
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR VulkanGraphicsImpl::ChooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanGraphicsImpl::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const
{
    int width;
    int height;
    SDL_GetWindowSize(mWindow, &width, &height);
    VkExtent2D actualExtent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };

    actualExtent.width = std::clamp(actualExtent.width,
                                    capabilities.minImageExtent.width,
                                    capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height,
                                     capabilities.minImageExtent.height,
                                     capabilities.maxImageExtent.height);

    return actualExtent;
}

bool VulkanGraphicsImpl::CheckDeviceExtensionSupport(
    VkPhysicalDevice aPhysicalDevice) const
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(aPhysicalDevice, nullptr,
                                         &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(aPhysicalDevice, nullptr,
                                         &extensionCount,
                                         availableExtensions.data());

    std::set<std::string> requiredExtensions(m_deviceExtensions.begin(),
                                             m_deviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

QueueFamilyIndices VulkanGraphicsImpl::FindQueueFamilies(
    VkPhysicalDevice device) const
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                             queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface,
                                             &presentSupport);

        if (presentSupport)
        {
            indices.mPresentFamily = i;
        }

        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.mGraphicsFamily = i;
        }

        if (indices.IsComplete())
        {
            break;
        }

        i++;
    }

    return indices;
}

SwapChainSupportDetails VulkanGraphicsImpl::QuerySwapChainSupport(
    VkPhysicalDevice device) const
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mSurface,
                                              &details.mCapabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount,
                                         nullptr);

    if (formatCount != 0)
    {
        details.mFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount,
                                             details.mFormats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface,
                                              &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.mPresentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device, mSurface, &presentModeCount, details.mPresentModes.data());
    }

    return details;
}

void VulkanGraphicsImpl::CreateLogicalDevice()
{
    Logger::Log(spdlog::level::debug, "Creating Logical Device");
    mFamilyIndices = FindQueueFamilies(mPhysicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {
        mFamilyIndices.mGraphicsFamily.value(),
        mFamilyIndices.mPresentFamily.value()
    };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    uint32_t graphicsFamilyIndex = mFamilyIndices.mGraphicsFamily.value();

    queueCreateInfo.queueFamilyIndex = graphicsFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.
        size());

    createInfo.pEnabledFeatures = &mDeviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.
        size());
    createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = mValidationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mLogicalDevice)
        != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(mLogicalDevice, mFamilyIndices.mPresentFamily.value(), 0,
                     &mPresentQueue);
    vkGetDeviceQueue(mLogicalDevice, graphicsFamilyIndex, 0, &mGraphicsQueue);
}

void VulkanGraphicsImpl::DestroyLogicalDevice()
{
    vkDestroyDevice(mLogicalDevice, nullptr);
}
