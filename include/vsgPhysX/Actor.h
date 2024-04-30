#pragma once

#include <foundation/PxSimpleTypes.h>

#include <vsg/core/ref_ptr.h>
#include <vsg/maths/vec4.h>

#include "Export.h"
#include "unique_ptr.h"

namespace physx
{
    class PxGeometry;
    class PxMaterial;
    class PxRigidActor;
} // namespace physx

namespace vsg
{
    class Node;
    class Builder;
} // namespace vsg

namespace vsgPhysX
{
    extern VSGPHYSX_DECLSPEC unique_ptr<physx::PxRigidActor> createActor(const physx::PxGeometry& geometry, physx::PxReal density, physx::PxMaterial* material = nullptr);

    extern VSGPHYSX_DECLSPEC unique_ptr<physx::PxRigidActor> createBoxActor(const vsg::vec3& dimensions, physx::PxReal density, physx::PxMaterial* material = nullptr);

    extern VSGPHYSX_DECLSPEC unique_ptr<physx::PxRigidActor> createPlaneActor(const vsg::vec4& plane, physx::PxMaterial* material = nullptr);

    extern VSGPHYSX_DECLSPEC vsg::ref_ptr<vsg::Node> createNodeForActor(vsg::Builder& builder, unique_ptr<physx::PxRigidActor>&& actor);
} // namespace vsgPhysX
