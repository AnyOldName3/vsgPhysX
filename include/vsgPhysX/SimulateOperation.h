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
    class SimulateOperation : public vsg::Inherit<vsg::Operation, SimulateOperation>
    {
    public:
        SimulateOperation(vsgPhysX::Scene* in_scene);

        vsg::observer_ptr<vsg::Viewer> viewer;
        vsg::ref_ptr<vsgPhysX::Scene> scene;

        double lastSimulationTime;

        void run() override;
    };
} // namespace vsgPhysX
