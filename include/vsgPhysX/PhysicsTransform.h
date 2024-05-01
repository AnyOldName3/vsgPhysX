#pragma once

#include <vsg/nodes/Transform.h>

#include "Export.h"
#include "ref_ptr.h"
#include "unique_ptr.h"

namespace physx
{
    class PxRigidActor;
}

namespace vsgPhysX
{
    // Transform that uses a PhysX actor's pose with no interpolation
    class VSGPHYSX_DECLSPEC PhysicsTransform : public vsg::Inherit<vsg::Transform, PhysicsTransform>
    {
    public:
        PhysicsTransform(unique_ptr<physx::PxRigidActor>&& in_actor = nullptr);

        unique_ptr<physx::PxRigidActor> actor;

        vsg::dmat4 transform(const vsg::dmat4& mv) const override;
    };
} // namespace vsgPhysX
