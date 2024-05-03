#pragma once

#include <vsg/utils/Builder.h>

#include "Export.h"

namespace physx
{
    class PxBoxGeometry;
    class PxCapsuleGeometry;
    class PxPlaneGeometry;
    class PxSphereGeometry;
} // namespace physx

namespace vsgPhysX
{
    // Transform that uses a PhysX actor's pose with no interpolation
    class VSGPHYSX_DECLSPEC ActorBuilder : public vsg::Inherit<vsg::Builder, ActorBuilder>
    {
    public:
        using vsg::Builder::createBox;
        using vsg::Builder::createCapsule;
        using vsg::Builder::createSphere;

        vsg::ref_ptr<vsg::Node> createBox(const physx::PxBoxGeometry& geometry, const vsg::mat4& transform = {}, const vsg::StateInfo& stateInfo = {});
        vsg::ref_ptr<vsg::Node> createCapsule(const physx::PxCapsuleGeometry& geometry, const vsg::mat4& transform = {}, const vsg::StateInfo& stateInfo = {});
        vsg::ref_ptr<vsg::Node> createPlane(const physx::PxPlaneGeometry& geometry, const vsg::mat4& transform = {}, const vsg::StateInfo& stateInfo = {});
        vsg::ref_ptr<vsg::Node> createSphere(const physx::PxSphereGeometry& geometry, const vsg::mat4& transform = {}, const vsg::StateInfo& stateInfo = {});

    protected:
        std::map<std::tuple<float, float, vsg::mat4, vsg::StateInfo>, vsg::ref_ptr<vsg::Node>> _physxCapsules;
    };
} // namespace vsgPhysX
