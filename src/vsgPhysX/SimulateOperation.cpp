#include "SimulateOperation.h"

#include <PxScene.h>

#include <vsg/app/Viewer.h>

vsgPhysX::SimulateOperation::SimulateOperation(vsgPhysX::Scene* in_scene) :
    scene(in_scene),
    lastSimulationTime(std::numeric_limits<double>::infinity())
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
            // TODO reusable scratch memory buffer
            scene->implementation().simulate(static_cast<physx::PxReal>(viewerTime - lastSimulationTime));
            scene->implementation().fetchResults(true);
            lastSimulationTime = viewerTime;
        }
        else if (lastSimulationTime == std::numeric_limits<double>::infinity())
        {
            lastSimulationTime = viewerTime;
        }
    }
}
