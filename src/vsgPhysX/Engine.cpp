#include "Engine.h"

#include "VsgVersionCheck.h"

#include <PxMaterial.h>
#include <PxPhysics.h>
#if VSG_API_VERSION_LESS(1, 1, 4)
#    include <extensions/PxDefaultAllocator.h>
#endif
#include <foundation/PxAllocatorCallback.h>
#include <foundation/PxErrorCallback.h>
#include <foundation/PxFoundation.h>
#include <foundation/PxPhysicsVersion.h>
#include <gpu/PxGpu.h>

#include <vsg/core/Allocator.h>
#include <vsg/io/Logger.h>

#include <iostream>
#include <stdexcept>

namespace
{
#if VSG_API_VERSION_LESS(1, 1, 4)
    physx::PxDefaultAllocator physXAllocator;
#else
    class VsgAllocatorCallback : public physx::PxAllocatorCallback
    {
        void* allocate(size_t size, const char* typeName, const char* filename, int line) override
        {
            return vsg::allocate(size, vsg::AllocatorAffinity::ALLOCATOR_AFFINITY_PHYSICS);
        }

        void deallocate(void* ptr) override
        {
            return vsg::deallocate(ptr);
        }
    };

    VsgAllocatorCallback physXAllocator;
#endif

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

physx::PxCudaContextManager* vsgPhysX::Engine::getOrCreateCudaContextManager(const physx::PxCudaContextManagerDesc& desc, physx::PxProfilerCallback* profilerCallback, bool launchSynchronous, bool force)
{
    if (force)
        _cudaContextManager.reset();

    if (!_cudaContextManager)
    {
        _cudaContextManager = unique_ptr<physx::PxCudaContextManager>(PxCreateCudaContextManager(*_foundation, desc, profilerCallback, launchSynchronous));
        if (_cudaContextManager && !_cudaContextManager->contextIsValid())
            _cudaContextManager.reset();
    }

    return _cudaContextManager.get();
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
