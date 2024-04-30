#include "PhysicsTransform.h"

#include <PxRigidActor.h>

#include "Convert.h"

vsgPhysX::PhysicsTransform::PhysicsTransform(unique_ptr<physx::PxRigidActor>&& in_actor) :
    actor(std::move(in_actor))
{
}

vsg::dmat4 vsgPhysX::PhysicsTransform::transform(const vsg::dmat4& mv) const
{
    return mv * vsg::dmat4(convert(physx::PxMat44(actor->getGlobalPose())));
}
