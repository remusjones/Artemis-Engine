//
// Created by Remus on 27/12/2023.
//

#include "LoadUtilities.h"

#include "tiny_obj_loader.h"
#include "Base/Common/Data/Mesh.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Logger.h"
#include "Vulkan/Helpers/VulkanInitialization.h"

bool LoadUtilities::LoadImageFromDisk(VulkanGraphics* aEngine, const char* aFilePath, AllocatedImage& aResult)
{
    int texWidth, texHeight, texChannels;

    stbi_uc* pixels = stbi_load(aFilePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    if (!pixels)
    {
        Logger::Log(spdlog::level::err, (std::string("Could not load file ") + aFilePath).c_str());
        return false;
    }

    void* pixel_ptr = pixels;
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    VkFormat image_format = VK_FORMAT_R8G8B8A8_SRGB;


    AllocatedBuffer stagingBuffer = AllocatedBuffer(pixel_ptr, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

    stbi_image_free(pixels);


    VkExtent3D imageExtent;
    imageExtent.width = static_cast<uint32_t>(texWidth);
    imageExtent.height = static_cast<uint32_t>(texHeight);
    imageExtent.depth = 1;

    VkImageCreateInfo dimg_info = VulkanInitialization::CreateImageInfo(image_format, VK_IMAGE_USAGE_SAMPLED_BIT |
                                                                        VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                                                                        imageExtent);

    AllocatedImage newImage;
    VmaAllocationCreateInfo dimg_allocinfo = {};
    dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    //allocate and create the image
    vmaCreateImage(aEngine->mAllocator, &dimg_info, &dimg_allocinfo, &newImage.mImage, &newImage.mAllocation, nullptr);

    aEngine->mVulkanEngine.SubmitBufferCommand([&](VkCommandBuffer cmd)
    {
        VkImageSubresourceRange range;
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        range.baseMipLevel = 0;
        range.levelCount = 1;
        range.baseArrayLayer = 0;
        range.layerCount = 1;

        VkImageMemoryBarrier imageBarrier_toTransfer = {};
        imageBarrier_toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

        imageBarrier_toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageBarrier_toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageBarrier_toTransfer.image = newImage.mImage;
        imageBarrier_toTransfer.subresourceRange = range;

        imageBarrier_toTransfer.srcAccessMask = 0;
        imageBarrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                             nullptr, 1, &imageBarrier_toTransfer);

        VkBufferImageCopy copyRegion = {};
        copyRegion.bufferOffset = 0;
        copyRegion.bufferRowLength = 0;
        copyRegion.bufferImageHeight = 0;

        copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.imageSubresource.mipLevel = 0;
        copyRegion.imageSubresource.baseArrayLayer = 0;
        copyRegion.imageSubresource.layerCount = 1;
        copyRegion.imageExtent = imageExtent;

        //copy the buffer into the image
        vkCmdCopyBufferToImage(cmd, stagingBuffer.mBuffer, newImage.mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                               &copyRegion);

        VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;

        imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
                             0, nullptr, 1, &imageBarrier_toReadable);
    });
    vmaDestroyBuffer(aEngine->mAllocator, stagingBuffer.mBuffer, stagingBuffer.mAllocation);
    aResult = newImage;
    return true;
}

bool LoadUtilities::CreateImage(const int aWidth, const int aHeight,
    VulkanGraphics *aEngine,
    AllocatedImage &aResult,
    Color_RGBA aColor = Color_RGBA(1,1,1,1)) {

    int texWidth = aWidth, texHeight = aHeight;  // Assuming RGB texture
    stbi_uc* pixels = new stbi_uc[4 * texWidth * texHeight];
    for (int i = 0; i < 4 * texWidth * texHeight; i += 4) {
        pixels[i] = aColor.R;     // red
        pixels[i + 1] = aColor.G; // green
        pixels[i + 2] = aColor.B; // blue
        pixels[i + 3] = aColor.A; // alpha
    }

    void* pixel_ptr = pixels;
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    VkFormat image_format = VK_FORMAT_R8G8B8A8_SRGB;


    AllocatedBuffer stagingBuffer = AllocatedBuffer(pixel_ptr, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    delete[] pixels;

    VkExtent3D imageExtent;
    imageExtent.width = static_cast<uint32_t>(texWidth);
    imageExtent.height = static_cast<uint32_t>(texHeight);
    imageExtent.depth = 1;

    VkImageCreateInfo dimg_info = VulkanInitialization::CreateImageInfo(image_format, VK_IMAGE_USAGE_SAMPLED_BIT |
                                                                        VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                                                                        imageExtent);

    AllocatedImage newImage;
    VmaAllocationCreateInfo dimg_allocinfo = {};
    dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    //allocate and create the image
    vmaCreateImage(aEngine->mAllocator, &dimg_info, &dimg_allocinfo, &newImage.mImage, &newImage.mAllocation, nullptr);

    aEngine->mVulkanEngine.SubmitBufferCommand([&](VkCommandBuffer cmd)
    {
        VkImageSubresourceRange range;
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        range.baseMipLevel = 0;
        range.levelCount = 1;
        range.baseArrayLayer = 0;
        range.layerCount = 1;

        VkImageMemoryBarrier imageBarrier_toTransfer = {};
        imageBarrier_toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

        imageBarrier_toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageBarrier_toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageBarrier_toTransfer.image = newImage.mImage;
        imageBarrier_toTransfer.subresourceRange = range;

        imageBarrier_toTransfer.srcAccessMask = 0;
        imageBarrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                             nullptr, 1, &imageBarrier_toTransfer);

        VkBufferImageCopy copyRegion = {};
        copyRegion.bufferOffset = 0;
        copyRegion.bufferRowLength = 0;
        copyRegion.bufferImageHeight = 0;

        copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.imageSubresource.mipLevel = 0;
        copyRegion.imageSubresource.baseArrayLayer = 0;
        copyRegion.imageSubresource.layerCount = 1;
        copyRegion.imageExtent = imageExtent;

        //copy the buffer into the image
        vkCmdCopyBufferToImage(cmd, stagingBuffer.mBuffer, newImage.mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                               &copyRegion);

        VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;

        imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
                             0, nullptr, 1, &imageBarrier_toReadable);
    });
    vmaDestroyBuffer(aEngine->mAllocator, stagingBuffer.mBuffer, stagingBuffer.mAllocation);
    aResult = newImage;
    return true;
}

bool LoadUtilities::LoadMeshFromDisk(const char* aFilePath, AllocatedVertexBuffer& aResult,
                                     std::vector<Vertex>& aResultVertices, std::vector<int16_t>& aResultIndices)
{
    //attrib will contain the vertex arrays of the file
    tinyobj::attrib_t attrib;
    //shapes contains the info for each separate object in the file
    std::vector<tinyobj::shape_t> shapes;
    //materials contains the information about the material of each shape, but we won't use it.
    std::vector<tinyobj::material_t> materials;

    //error and warning output from the load function
    std::string warn;
    std::string err;

    //load the OBJ file
    LoadObj(&attrib, &shapes, &materials, &warn, &err, aFilePath, nullptr);

    if (!warn.empty())
    {
        Logger::Log(spdlog::level::warn, warn.c_str());
    }
    //if we have any error, print it to the console, and break the mesh loading.
    //This happens if the file can't be found or is malformed
    if (!err.empty())
    {
        Logger::Log(spdlog::level::err, err.c_str());
        return false;
    }


    for (size_t s = 0; s < shapes.size(); s++)
    {
        glm::vec3 tangent;

        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            //hardcode loading to triangles
            int fv = 3;
            if (f % 3 == 0 && f > 0)
            {
                const glm::vec3 &pos1 = aResultVertices[aResultIndices[f - 3]].mPosition;
                const glm::vec3 &pos2 = aResultVertices[aResultIndices[f - 2]].mPosition;
                const glm::vec3 &pos3 = aResultVertices[aResultIndices[f - 1]].mPosition;

                const glm::vec2 &uv1 = aResultVertices[aResultIndices[f - 3]].mUV;
                const glm::vec2 &uv2 = aResultVertices[aResultIndices[f - 2]].mUV;
                const glm::vec2 &uv3 = aResultVertices[aResultIndices[f - 1]].mUV;

                glm::vec3 edge1 = pos2 - pos1;
                glm::vec3 edge2 = pos3 - pos1;
                glm::vec2 deltaUV1 = uv2 - uv1;
                glm::vec2 deltaUV2 = uv3 - uv1;

                float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

                tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
                tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
                tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

                tangent = glm::normalize(tangent);
            }

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++)
            {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                //vertex position
                tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
                //vertex normal
                tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
                tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
                tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];

                //copy it into our vertex
                Vertex newVertex;
                newVertex.mPosition.x = vx;
                newVertex.mPosition.y = vy;
                newVertex.mPosition.z = vz;

                newVertex.mNormal.x = nx;
                newVertex.mNormal.y = ny;
                newVertex.mNormal.z = nz;

                //we are setting the vertex color as the vertex normal. This is just for display purposes
                newVertex.mColor = newVertex.mNormal;
                newVertex.mTangent = tangent;

                aResultIndices.push_back(idx.vertex_index);
                aResultVertices.push_back(newVertex);
            }
            index_offset += fv;
        }
    }
    aResult = AllocatedVertexBuffer(aResultVertices, aResultIndices);
    return true;
}
