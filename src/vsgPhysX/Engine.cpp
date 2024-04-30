#include "Engine.h"

#include <PxMaterial.h>
#include <PxPhysics.h>
#include <extensions/PxDefaultAllocator.h>
#include <foundation/PxErrorCallback.h>
#include <foundation/PxFoundation.h>
#include <foundation/PxPhysicsVersion.h>

#include <iostream>
#include <stdexcept>

namespace
{
    // TODO: Make these configurable, or at least VSG-based.
    // I guess it's sensible to unconditionally wire logging to VSG's logging and adapt VSG's allocator with a PhysX-specific affinity as the default PhysX allocator doesn't have any anti-fragmentation measures.
    physx::PxDefaultAllocator physXAllocator;

    class ErrorCallback : public physx::PxErrorCallback
    {
    public:
        ~ErrorCallback() override = default;

        void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override{};
    };

    ErrorCallback errorCallback;
} // namespace

const std::unique_ptr<vsgPhysX::Engine>& vsgPhysX::Engine::reset(const physx::PxTolerancesScale& tolerancesScale)
{
    instanceInternal() = std::unique_ptr<Engine>(new Engine(tolerancesScale));
    return instance();
}

const std::unique_ptr<vsgPhysX::Engine>& vsgPhysX::Engine::instance()
{
    return instanceInternal();
}

std::unique_ptr<vsgPhysX::Engine>& vsgPhysX::Engine::instanceInternal()
{
    static std::unique_ptr<Engine> s_engine{nullptr};
    return s_engine;
}

// TODO extra PxCreatePhysics parameters
vsgPhysX::Engine::Engine(const physx::PxTolerancesScale& tolerancesScale) :
    _foundation(PxCreateFoundation(PX_PHYSICS_VERSION, physXAllocator, errorCallback)), _physics(PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, tolerancesScale, false, nullptr, nullptr))
{
    std::cout << "creating Engine" << std::endl;
    defaultMaterial = _physics->createMaterial(0.5f, 0.5f, 0.5f);
}

vsgPhysX::Engine::~Engine()
{
    std::cout << "destroying Engine" << std::endl;
}
