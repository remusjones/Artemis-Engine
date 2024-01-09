//
// Created by Remus on 17/12/2023.
//

#pragma once


#include "Base/Common/Buffers/PushConstants.h"
#include "Base/Common/Data/Mesh.h"
#include "Base/Common/Data/Vertex.h"
#include "Math/Transform.h"
#include "Objects/ImGuiLayer.h"
#include "Objects/Super.h"
#include "Vulkan/Renderers/MeshRenderer.h"
#include "Vulkan/Renderers/Renderer.h"

class Scene;
class Mesh;
class Material;
class VulkanEngine;
class GraphicsPipeline;
class Buffer;
class CameraInformationBuffer;
class AllocatedVertexBuffer;


/*Attempts to abstract the required components for rendering to
 * identify what can be seperated from render pipeline */
class MeshObject : public Super, public ImGuiLayer {
public:
    void Construct() override;
    void Tick(float aDeltaTime) override;
    void Cleanup() override;
    void OnImGuiRender() override;
    void CreateObject(Material &aMaterial, const char *aName = "Default");


    MeshRenderer mMeshRenderer;
    Transform mTransform;
};
