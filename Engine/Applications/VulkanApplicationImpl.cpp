//
// Created by Remus on 4/11/2021.
//
#include "VulkanApplicationImpl.h"
#include "Common/VulkanRenderer.h"
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <set>
#include <cstdint>
#include <algorithm>
#include <VulkanSwapChain.h>

void VulkanApplicationImpl::Run()
{
    InitializeWindow();
    InitializeVulkan();
    Update();
    Cleanup();
}

void VulkanApplicationImpl::InitializeVulkan()
{
    CreateInstance();
    SetupDebugMessenger();
    CreateSurface();
    InitializePhysicalDevice();
    CreateLogicalDevice();
    mSwapChain = new VulkanSwapChain();
    mSwapChain->Initialize(mLogicalDevice,
                           mPhysicalDevice,
                           mSurface,
                           mRenderPass,
                           this);
    CreateGraphicsPipeline();
}

void VulkanApplicationImpl::Update()
{
    while (!glfwWindowShouldClose(mWindow))
    {
        glfwPollEvents();
        this->mRenderPipeline.DrawFrame();
    }
}

void VulkanApplicationImpl::Cleanup()
{

    vkDeviceWaitIdle(mLogicalDevice);
    mRenderPipeline.Cleanup();
    mSwapChain->Cleanup();
    delete mSwapChain;
    vkDestroyDevice(mLogicalDevice, nullptr);

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(mVulkanInstance, mDebugMessenger, nullptr);
    }
    vkDestroySurfaceKHR(mVulkanInstance, mSurface, nullptr);
    vkDestroyInstance(mVulkanInstance, nullptr);

    std::cout << "Destroying VulkanApplicationImpl" << std::endl;
    glfwDestroyWindow(mWindow);
    glfwTerminate();

}

VulkanApplicationImpl::VulkanApplicationImpl(const char *aWindowName, int aWindowWidth, int aWindowHeight)
{
    mWindowName = aWindowName;
    mWindowWidth = aWindowWidth;
    mWindowHeight = aWindowHeight;
}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {

    auto app = reinterpret_cast<VulkanApplicationImpl*>(glfwGetWindowUserPointer(window));
    app->mRenderPipeline.mFramebufferResized = true;

    std::cout << "Window Resized: x: " << width << " y: " << height << std::endl;
}

void VulkanApplicationImpl::InitializeWindow()
{
    std::cout << "Creating Window" << std::endl;

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    mWindow = glfwCreateWindow(mWindowWidth, mWindowHeight, mWindowName, nullptr, nullptr);
    glfwSetWindowUserPointer(mWindow, this);
    glfwSetFramebufferSizeCallback(mWindow, framebufferResizeCallback);

}

void VulkanApplicationImpl::CreateInstance()
{
    std::cout << "Creating Vulkan Instance" << std::endl;

    if (enableValidationLayers && !CheckValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
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
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
        createInfo.ppEnabledLayerNames = mValidationLayers.data();

        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;

    } else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    auto extensions = GetRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    if (vkCreateInstance(&createInfo, nullptr, &mVulkanInstance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }else
    {
        std::cout << "Vulkan Instance Created Successfully" << std::endl;
    }

}

void VulkanApplicationImpl::CreateSurface()
{
    if (glfwCreateWindowSurface(mVulkanInstance, mWindow, nullptr, &mSurface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}


// Have this function virtual for extension??
void VulkanApplicationImpl::CreateGraphicsPipeline()
{


    mRenderPipeline.Initialize(mLogicalDevice,
                               mSwapChain,
                               mPhysicalDevice,
                               mGraphicsQueue,
                               mPresentQueue
                              );



    // Create Triangle Renderer
    VulkanRenderer triangleRenderer;
    triangleRenderer.material = mRenderPipeline.LoadShader("triangleInput"); // Create Material

    const std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f}, {.0f, 0.0f, 1.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 0.5f, 1.0f}}
    };


    mRenderPipeline.CreatePipelineLayout();
    auto queueFamilies = FindQueueFamilies(mPhysicalDevice);
    mRenderPipeline.CreateCommandPool(queueFamilies);
    mRenderPipeline.CreateVertexBuffer(vertices);
    mRenderPipeline.CreateIndexBuffer();


    mRenderPipeline.CreateSyncObjects();
}

bool VulkanApplicationImpl::CheckValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : mValidationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

std::vector<const char *> VulkanApplicationImpl::GetRequiredExtensions() const{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }

    return extensions;
}


void VulkanApplicationImpl::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& aCreateInfo) {
    aCreateInfo = {};
    aCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    aCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    aCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    aCreateInfo.pfnUserCallback = DebugCallback;
}


void VulkanApplicationImpl::SetupDebugMessenger()
{
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    PopulateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(mVulkanInstance, &createInfo, nullptr, &mDebugMessenger) != VK_SUCCESS)
    {
        std::cout << "failed to set up debug messenger!" << std::endl;
    }

}
VkResult VulkanApplicationImpl::CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                             const VkDebugUtilsMessengerCreateInfoEXT *aCreateInfo,
                                                             const VkAllocationCallbacks *aAllocator,
                                                             VkDebugUtilsMessengerEXT *aDebugMessenger)
{

    PFN_vkCreateDebugUtilsMessengerEXT func =
            reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                    vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT" )
                    );

    if (func != nullptr)
    {
        return func(instance, aCreateInfo, aAllocator, aDebugMessenger);
    }else
        return VK_ERROR_EXTENSION_NOT_PRESENT;

}

void VulkanApplicationImpl::DestroyDebugUtilsMessengerEXT(VkInstance aInstance, VkDebugUtilsMessengerEXT aDebugMessenger,
                                                          const VkAllocationCallbacks *aAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(aInstance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(aInstance, aDebugMessenger, aAllocator);
    }
}

VkBool32 VulkanApplicationImpl::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT aMessageSeverity,
                                              VkDebugUtilsMessageTypeFlagsEXT aMessageType,
                                              const VkDebugUtilsMessengerCallbackDataEXT *aCallbackData, void *aUserData)
{
    if (aMessageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        std::cerr << aMessageType << " Validation layer: " <<
                  aCallbackData->pMessage <<
                  std::endl;
    }
    return VK_FALSE;
}

void VulkanApplicationImpl::InitializePhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(mVulkanInstance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(mVulkanInstance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        if (IsDeviceSuitable(device)) {
            mPhysicalDevice = device;
            break;
        }
    }

    if (mPhysicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }else
    {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(mPhysicalDevice, &deviceProperties);
        vkGetPhysicalDeviceFeatures(mPhysicalDevice, &deviceFeatures);

        std::cout << "Selecting Device: " << deviceProperties.deviceName << std::endl;
    }
}

bool VulkanApplicationImpl::IsDeviceSuitable(VkPhysicalDevice aPhysicalDevice)
{
    QueueFamilyIndices indices = FindQueueFamilies(aPhysicalDevice);

    bool extensionsSupported = CheckDeviceExtensionSupport(aPhysicalDevice);

    bool swapChainAdequate = false;

    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(aPhysicalDevice);
        swapChainAdequate = !swapChainSupport.mFormats.empty() && !swapChainSupport.mPresentModes.empty();
    }

    return indices.IsComplete() && extensionsSupported && swapChainAdequate;
}

VkSurfaceFormatKHR VulkanApplicationImpl::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}
VkPresentModeKHR VulkanApplicationImpl::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D VulkanApplicationImpl::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(mWindow, &width, &height);

        VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

bool VulkanApplicationImpl::CheckDeviceExtensionSupport(VkPhysicalDevice aPhysicalDevice)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(aPhysicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(aPhysicalDevice, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

QueueFamilyIndices VulkanApplicationImpl::FindQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface, &presentSupport);

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

SwapChainSupportDetails VulkanApplicationImpl::QuerySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mSurface, &details.mCapabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.mFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, details.mFormats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.mPresentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, details.mPresentModes.data());
    }

    return details;

}

void VulkanApplicationImpl::CreateLogicalDevice()
{
    std::cout << "Creating Logical Device" << std::endl;
    QueueFamilyIndices indices = FindQueueFamilies(mPhysicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.mGraphicsFamily.value(), indices.mPresentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }


    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    uint32_t graphicsFamilyIndex = indices.mGraphicsFamily.value();

    queueCreateInfo.queueFamilyIndex = graphicsFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

    createInfo.pEnabledFeatures = &mDeviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

    //TODO: validation layers deprecated on devices, remove?

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = mValidationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mLogicalDevice) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(mLogicalDevice, indices.mPresentFamily.value(), 0, &mPresentQueue);
    vkGetDeviceQueue(mLogicalDevice, graphicsFamilyIndex, 0, &mGraphicsQueue);

}

