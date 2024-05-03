#include <PxPhysics.h>
#include <PxRigidActor.h>
#include <PxScene.h>
#include <extensions/PxDefaultCpuDispatcher.h>
#include <extensions/PxDefaultSimulationFilterShader.h>

#include <vsg/all.h>

#include <vsgPhysX/Actor.h>
#include <vsgPhysX/ActorBuilder.h>
#include <vsgPhysX/Convert.h>
#include <vsgPhysX/Engine.h>
#include <vsgPhysX/Scene.h>
#include <vsgPhysX/SimulateOperation.h>

#include <iostream>

int main(int argc, char** argv)
{
    // set up defaults and read command line arguments to override them
    vsg::CommandLine arguments(&argc, argv);

    // set up defaults and read command line arguments to override them
    auto options = vsg::Options::create();
    options->sharedObjects = vsg::SharedObjects::create();
    options->fileCache = vsg::getEnv("VSG_FILE_CACHE");
    options->paths = vsg::getEnvPaths("VSG_FILE_PATH");

#ifdef vsgXchange_all
    // add vsgXchange's support for reading and writing 3rd party file formats
    options->add(vsgXchange::all::create());
#endif

    arguments.read(options);

    auto windowTraits = vsg::WindowTraits::create();
    windowTraits->windowTitle = "vsgPhysX Hello World";

    windowTraits->debugLayer = arguments.read({"--debug", "-d"});
    windowTraits->apiDumpLayer = arguments.read({"--api", "-a"});

    arguments.read("--screen", windowTraits->screenNum);
    arguments.read("--display", windowTraits->display);
    auto numFrames = arguments.value(-1, "-f");
    if (arguments.read({"--fullscreen", "--fs"})) windowTraits->fullscreen = true;
    if (arguments.read({"--window", "-w"}, windowTraits->width, windowTraits->height)) { windowTraits->fullscreen = false; }
    if (arguments.read("--IMMEDIATE")) { windowTraits->swapchainPreferences.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR; }
    if (arguments.read({"-t", "--test"}))
    {
        windowTraits->swapchainPreferences.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        windowTraits->fullscreen = true;
    }
    if (arguments.read("--st"))
    {
        windowTraits->swapchainPreferences.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        windowTraits->width = 192, windowTraits->height = 108;
        windowTraits->decoration = false;
    }

    vsg::ref_ptr<vsg::Instrumentation> instrumentation;
    if (arguments.read({"--gpu-annotation", "--ga"}) && vsg::isExtensionSupported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
    {
        windowTraits->debugUtils = true;

        auto gpu_instrumentation = vsg::GpuAnnotation::create();
        if (arguments.read("--func")) gpu_instrumentation->labelType = vsg::GpuAnnotation::SourceLocation_function;

        instrumentation = gpu_instrumentation;
    }
    else if (arguments.read({"--profiler", "--pr"}))
    {
        // set Profiler options
        auto settings = vsg::Profiler::Settings::create();
        arguments.read("--cpu", settings->cpu_instrumentation_level);
        arguments.read("--gpu", settings->gpu_instrumentation_level);
        arguments.read("--log-size", settings->log_size);
        arguments.read("--gpu-size", settings->gpu_timestamp_size);

        // create the profiler
        instrumentation = vsg::Profiler::create(settings);
    }
#ifdef Tracy_FOUND
    else if (arguments.read("--tracy"))
    {
        windowTraits->deviceExtensionNames.push_back(VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME);

        auto tracy_instrumentation = vsg::TracyInstrumentation::create();
        arguments.read("--cpu", tracy_instrumentation->settings->cpu_instrumentation_level);
        arguments.read("--gpu", tracy_instrumentation->settings->gpu_instrumentation_level);
        instrumentation = tracy_instrumentation;
    }
#endif

    unsigned int numLights = 2;
    auto direction = arguments.value(vsg::dvec3(-0.25, 0.25, -1.0), "--direction");
    auto lambda = arguments.value<double>(0.25, "--lambda");
    double maxShadowDistance = arguments.value<double>(100, "--sd");
    double nearFarRatio = arguments.value<double>(0.001, "--nf");
    vsg::ref_ptr<vsg::ShadowSettings> shadowSettings;

    auto numShadowMapsPerLight = arguments.value<uint32_t>(3, "--sm");
    if (numShadowMapsPerLight > 0)
    {
        auto shaderHints = vsg::ShaderCompileSettings::create();

        float penumbraRadius = 0.005;
        if (arguments.read("--pcss"))
        {
            shadowSettings = vsg::PercentageCloserSoftShadows::create(numShadowMapsPerLight);
            shaderHints->defines.insert("VSG_SHADOWS_PCSS");
        }

        if (arguments.read("--soft", penumbraRadius))
        {
            shadowSettings = vsg::SoftShadows::create(numShadowMapsPerLight, penumbraRadius);
            shaderHints->defines.insert("VSG_SHADOWS_SOFT");
        }

        if (arguments.read("--hard") || !shadowSettings)
        {
            shadowSettings = vsg::HardShadows::create(numShadowMapsPerLight);
            // shaderHints->defines.insert("VSG_SHADOWS_HARD");
        }

        std::cout << "Enabled depth clamp." << std::endl;
        auto deviceFeatures = windowTraits->deviceFeatures = vsg::DeviceFeatures::create();
        deviceFeatures->get().samplerAnisotropy = VK_TRUE;
        deviceFeatures->get().depthClamp = VK_TRUE;

        if (arguments.read("--shader-debug"))
        {
            shaderHints->defines.insert("SHADOWMAP_DEBUG");
        }

        auto rasterizationState = vsg::RasterizationState::create();
        rasterizationState->depthClampEnable = VK_TRUE;

        auto pbr = options->shaderSets["pbr"] = vsg::createPhysicsBasedRenderingShaderSet(options);
        pbr->defaultGraphicsPipelineStates.push_back(rasterizationState);
        pbr->defaultShaderHints = shaderHints;
        pbr->variants.clear();

        auto phong = options->shaderSets["phong"] = vsg::createPhongShaderSet(options);
        phong->defaultGraphicsPipelineStates.push_back(rasterizationState);
        phong->defaultShaderHints = shaderHints;
        phong->variants.clear();

        auto flat = options->shaderSets["flat"] = vsg::createPhysicsBasedRenderingShaderSet(options);
        flat->defaultGraphicsPipelineStates.push_back(rasterizationState);
        flat->defaultShaderHints = shaderHints;
        flat->variants.clear();
    }

    physx::PxGeometryType::Enum geometryType = physx::PxGeometryType::eBOX;
    if (arguments.read("--sphere"))
        geometryType = physx::PxGeometryType::eSPHERE;
    if (arguments.read("--capsule"))
        geometryType = physx::PxGeometryType::eCAPSULE;
    if (arguments.read("--box"))
        geometryType = physx::PxGeometryType::eBOX;

    vsgPhysX::Engine::reset(physx::PxTolerancesScale{1.0f, 10.0f});

    physx::PxSceneDesc sceneDesc{vsgPhysX::Engine::instance()->physics().getTolerancesScale()};
    sceneDesc.gravity = {0.0f, 0.0f, -9.81f};
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    auto physicsScene = vsgPhysX::Scene::create(sceneDesc);

    auto vsgScene = vsg::Group::create();
    auto builder = vsgPhysX::ActorBuilder::create();

    // Create the ground
    vsgPhysX::unique_ptr<physx::PxRigidActor> planeActor = vsgPhysX::createPlaneActor(vsg::vec4(0.0f, 0.0f, 1.0f, 0.0f));
    physicsScene->addActor(planeActor);

    vsgScene->addChild(vsgPhysX::createNodeForActor(*builder, std::move(planeActor)));

    // Create many cubes
    for (int y = 0; y < 10; ++y)
    {
        for (int x = 0; x < 10; ++x)
        {
            vsgPhysX::unique_ptr<physx::PxRigidActor> actor;
            //
            switch (geometryType)
            {
            case physx::PxGeometryType::eSPHERE:
                actor = vsgPhysX::createSphereActor(0.5f, 1.0f);
                break;
            case physx::PxGeometryType::ePLANE:
                break;
            case physx::PxGeometryType::eCAPSULE:
                actor = vsgPhysX::createCapsuleActor(0.2f, 0.4f, 1.0f);
                break;
            case physx::PxGeometryType::eBOX:
                actor = vsgPhysX::createBoxActor({1.0f, 1.0f, 1.0f}, 1.0f);
                break;
            case physx::PxGeometryType::eCONVEXMESH:
                break;
            case physx::PxGeometryType::ePARTICLESYSTEM:
                break;
            case physx::PxGeometryType::eTETRAHEDRONMESH:
                break;
            case physx::PxGeometryType::eTRIANGLEMESH:
                break;
            case physx::PxGeometryType::eHEIGHTFIELD:
                break;
            case physx::PxGeometryType::eHAIRSYSTEM:
                break;
            case physx::PxGeometryType::eCUSTOM:
                break;
            default:
                break;
            }
            actor->setGlobalPose(physx::PxTransform(vsgPhysX::convert(vsg::translate(1.05f * float(x), ((x * y) % 7) > 3 ? 0.0f : 0.01f, 1.5f * float(y)))));
            physicsScene->addActor(actor);

            vsgScene->addChild(vsgPhysX::createNodeForActor(*builder, std::move(actor)));
        }
    }

    vsg::ref_ptr<vsgPhysX::SimulateOperation> simulateOperation = vsgPhysX::SimulateOperation::create(physicsScene, 16);

    auto bounds = vsg::visit<vsg::ComputeBounds>(vsgScene).bounds;

    vsg::ref_ptr<vsg::LookAt> lookAt = vsg::LookAt::create(vsg::dvec3(15, -50, 22.5), (bounds.min + bounds.max) / 2.0, vsg::dvec3(0, 0, 1));

    if (numLights >= 1)
    {
        auto directionalLight = vsg::DirectionalLight::create();
        directionalLight->name = "directional";
        directionalLight->color.set(1.0, 1.0, 1.0);
        directionalLight->intensity = 0.9;
        directionalLight->direction = direction;
        directionalLight->shadowSettings = shadowSettings;

        vsgScene->addChild(directionalLight);
    }
    if (numLights >= 2)
    {
        auto ambientLight = vsg::AmbientLight::create();
        ambientLight->name = "ambient";
        ambientLight->color.set(1.0, 1.0, 1.0);
        ambientLight->intensity = 0.1;
        vsgScene->addChild(ambientLight);
    }

    // create the viewer and assign window(s) to it
    auto viewer = vsg::Viewer::create();

    auto window = vsg::Window::create(windowTraits);
    if (!window)
    {
        std::cout << "Could not create window." << std::endl;
        return 1;
    }

    viewer->addWindow(window);

    // set up the camera
    vsg::ref_ptr<vsg::ProjectionMatrix> perspective = vsg::Perspective::create(30.0, static_cast<double>(window->extent2D().width) / static_cast<double>(window->extent2D().height), nearFarRatio * 100, 100 * 1.5);

    auto camera = vsg::Camera::create(perspective, lookAt, vsg::ViewportState::create(window->extent2D()));

    // add the camera and scene graph to View
    auto view = vsg::View::create();
    view->viewDependentState->maxShadowDistance = maxShadowDistance;
    view->viewDependentState->lambda = lambda;
    view->camera = camera;
    view->addChild(vsgScene);

    // add close handler to respond to the close window button and pressing escape
    viewer->addEventHandler(vsg::CloseHandler::create(viewer));
    viewer->addEventHandler(vsg::Trackball::create(camera));

    viewer->addUpdateOperation(simulateOperation, vsg::UpdateOperations::ALL_FRAMES);
    simulateOperation->viewer = viewer;

    auto renderGraph = vsg::RenderGraph::create(window, view);
    auto commandGraph = vsg::CommandGraph::create(window, renderGraph);
    viewer->assignRecordAndSubmitTaskAndPresentation({commandGraph});

    if (instrumentation) viewer->assignInstrumentation(instrumentation);

    auto before_compile = vsg::clock::now();

    viewer->compile();

    std::cout << "Compile time : " << std::chrono::duration<double, std::chrono::seconds::period>(vsg::clock::now() - before_compile).count() * 1000.0 << " ms" << std::endl;

    auto startTime = vsg::clock::now();
    double numFramesCompleted = 0.0;

    double updateTime = 0.0;

    // rendering main loop
    while (viewer->advanceToNextFrame() && (numFrames < 0 || (numFrames--) > 0))
    {
        // pass any events into EventHandlers assigned to the Viewer
        viewer->handleEvents();

        auto before = vsg::clock::now();

        viewer->update();

        updateTime += std::chrono::duration<double, std::chrono::seconds::period>(vsg::clock::now() - before).count();

        viewer->recordAndSubmit();
        viewer->present();

        numFramesCompleted += 1.0;
    }

    auto duration = std::chrono::duration<double, std::chrono::seconds::period>(vsg::clock::now() - startTime).count();
    if (numFramesCompleted > 0.0)
    {
        std::cout << "Average frame rate = " << (numFramesCompleted / duration) << std::endl;
        std::cout << "Average update time = " << (updateTime / numFramesCompleted) * 1000.0 << " ms" << std::endl;
    }

    if (auto profiler = instrumentation.cast<vsg::Profiler>())
    {
        instrumentation->finish();
        profiler->log->report(std::cout);
    }

    return 0;
}