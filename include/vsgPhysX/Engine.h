#pragma once

#include <common/PxTolerancesScale.h>

#include <vsg/core/Inherit.h>

#include "Export.h"
#include "ref_ptr.h"
#include "unique_ptr.h"

namespace physx
{
    class PxFoundation;
    class PxMaterial;
    class PxPhysics;
} // namespace physx

namespace vsgPhysX
{
    class VSGPHYSX_DECLSPEC Engine
    {
    public:
        static const std::unique_ptr<Engine>& reset(const physx::PxTolerancesScale& tolerancesScale = physx::PxTolerancesScale());

        static const std::unique_ptr<Engine>& instance();

    protected:
        // put these at the top so they outlive everything else
        unique_ptr<physx::PxFoundation> _foundation;
        unique_ptr<physx::PxPhysics> _physics;

    public:
        virtual ~Engine();

        ref_ptr<physx::PxMaterial> defaultMaterial;

        physx::PxFoundation& foundation() const { return *_foundation; }
        physx::PxPhysics& physics() const { return *_physics; }

    protected:
        static std::unique_ptr<Engine>& instanceInternal();

        Engine(const physx::PxTolerancesScale& tolerancesScale);
    };
} // namespace vsgPhysX
