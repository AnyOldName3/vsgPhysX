#include "Scene.h"

#include <PxPhysics.h>
#include <PxScene.h>

#include "Engine.h"

vsgPhysX::Scene::Scene(const physx::PxSceneDesc& sceneDesc) :
    _implementation(Engine::instance()->physics().createScene(sceneDesc))
{
}

bool vsgPhysX::Scene::addActor(physx::PxActor& actor, const physx::PxBVH* bvh)
{
    return implementation().addActor(actor, bvh);
}

vsgPhysX::Scene::~Scene() = default;
