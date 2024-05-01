#include "Engine.h"

#include <PxMaterial.h>
#include <PxPhysics.h>
#include <extensions/PxDefaultAllocator.h>
#include <foundation/PxErrorCallback.h>
#include <foundation/PxFoundation.h>
#include <foundation/PxPhysicsVersion.h>

#include <vsg/io/Logger.h>

#include <iostream>
#include <stdexcept>

namespace
{
    // TODO: Make this configurable, or at least VSG-based.
    // I guess it's sensible to adapt VSG's allocator with a PhysX-specific affinity as the default PhysX allocator doesn't have any anti-fragmentation measures.
    physx::PxDefaultAllocator physXAllocator;

    class ErrorCallback : public physx::PxErrorCallback
    {
    public:
        ~ErrorCallback() override = default;

        void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override
        {
            vsg::Logger::Level level = vsg::Logger::LOGGER_DEBUG;
            switch (code)
            {
            case physx::PxErrorCode::eNO_ERROR:
                level = vsg::Logger::LOGGER_DEBUG;
                break;
            case physx::PxErrorCode::eDEBUG_INFO:
                level = vsg::Logger::LOGGER_INFO;
                break;
            case physx::PxErrorCode::eDEBUG_WARNING:
                level = vsg::Logger::LOGGER_WARN;
                break;
            case physx::PxErrorCode::eINVALID_PARAMETER:
                level = vsg::Logger::LOGGER_ERROR;
                break;
            case physx::PxErrorCode::eINVALID_OPERATION:
                level = vsg::Logger::LOGGER_ERROR;
                break;
            case physx::PxErrorCode::eOUT_OF_MEMORY:
                level = vsg::Logger::LOGGER_ERROR;
                break;
            case physx::PxErrorCode::eINTERNAL_ERROR:
                level = vsg::Logger::LOGGER_ERROR;
                break;
            case physx::PxErrorCode::eABORT:
                level = vsg::Logger::LOGGER_FATAL;
                break;
            case physx::PxErrorCode::ePERF_WARNING:
                level = vsg::Logger::LOGGER_WARN;
                break;
            default:
                break;
            }
            vsg::log(level, "PhysX ", file, " line ", line, ": ", message);
        };
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
    defaultMaterial = _physics->createMaterial(0.5f, 0.5f, 0.5f);
}

vsgPhysX::Engine::~Engine()
{
}
