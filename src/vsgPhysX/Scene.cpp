#include "Scene.h"

#include <PxPhysics.h>
#include <PxScene.h>

#include "Engine.h"

vsgPhysX::Scene::Scene(const physx::PxSceneDesc& sceneDesc) :
    _implementation(Engine::instance()->physics().createScene(sceneDesc))
{
}

vsgPhysX::Scene::~Scene() = default;
