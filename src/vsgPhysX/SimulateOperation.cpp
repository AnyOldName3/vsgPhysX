#include "SimulateOperation.h"

#include <PxScene.h>

#include <vsg/app/Viewer.h>

vsgPhysX::SimulateOperation::SimulateOperation(vsgPhysX::Scene* in_scene, std::size_t scratchMemoryBlockCount) :
    scene(in_scene),
    lastSimulationTime(std::numeric_limits<double>::infinity()),
    scratchMemory(scratchMemoryBlockCount)
{
}

void vsgPhysX::SimulateOperation::run()
{
    vsg::ref_ptr<vsg::Viewer> refViewer = viewer;
    if (refViewer)
    {
        double viewerTime = refViewer->getFrameStamp()->simulationTime;
        if (lastSimulationTime < viewerTime)
        {
            scene->implementation().simulate(static_cast<physx::PxReal>(viewerTime - lastSimulationTime), nullptr, scratchMemory.data(), scratchBufferSize(scratchMemory));
            scene->implementation().fetchResults(true);
            lastSimulationTime = viewerTime;
        }
        else if (lastSimulationTime == std::numeric_limits<double>::infinity())
        {
            lastSimulationTime = viewerTime;
        }
    }
}

void vsgPhysX::SimulateOperation::resizeScratchMemory(std::size_t blockCount)
{
    scratchMemory.resize(blockCount);
}
