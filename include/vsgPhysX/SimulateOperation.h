#pragma once

#include <vsg/core/Inherit.h>
#include <vsg/core/observer_ptr.h>
#include <vsg/threading/OperationQueue.h>

#include "Export.h"
#include "Scene.h"

namespace vsg
{
    class Viewer;
}

namespace vsgPhysX
{
    // Naive operation to run the simulation for the time delta between the viewer's current simulation time and the last one this operation was run for.
    // Expect poor numerical stability and nondeterminism if the simulation time does not have a fixed increment.
    class SimulateOperation : public vsg::Inherit<vsg::Operation, SimulateOperation>
    {
    public:
        SimulateOperation(vsgPhysX::Scene* in_scene, std::size_t scratchMemoryBlockCount = 0);

        vsg::observer_ptr<vsg::Viewer> viewer;
        vsg::ref_ptr<vsgPhysX::Scene> scene;

        double lastSimulationTime;

        void run() override;

        void resizeScratchMemory(std::size_t blockCount);

    private:
        class alignas(16) ScratchBlock
        {
            std::uint8_t buffer[16 * 1024];
        };

        std::vector<ScratchBlock> scratchMemory;
    };
} // namespace vsgPhysX
