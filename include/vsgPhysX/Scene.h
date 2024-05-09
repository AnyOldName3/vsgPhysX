#pragma once

#include <PxSceneDesc.h>

#include <vsg/core/Inherit.h>

#include "Export.h"
#include "unique_ptr.h"

namespace physx
{
    class PxBVH;
    class PxScene;
} // namespace physx

namespace vsgPhysX
{
    class VSGPHYSX_DECLSPEC Scene : public vsg::Inherit<vsg::Object, Scene>
    {
    public:
        Scene(const physx::PxSceneDesc& sceneDesc);

        physx::PxScene& implementation() const { return *_implementation; }

        template<class T>
        bool addActor(unique_ptr<T>& actor, const physx::PxBVH* bvh = nullptr)
        {
            if (actor)
                return addActor(*actor, bvh);
            return false;
        }

        bool addActor(physx::PxActor& actor, const physx::PxBVH* bvh = nullptr);

    protected:
        virtual ~Scene();

        unique_ptr<physx::PxScene> _implementation;
    };
} // namespace vsgPhysX
