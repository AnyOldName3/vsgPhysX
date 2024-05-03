#include "Actor.h"

#include <PxRigidDynamic.h>
#include <PxRigidStatic.h>
#include <extensions/PxSimpleFactory.h>
#include <geometry/PxPlaneGeometry.h>

#include "ActorBuilder.h"
#include "Convert.h"
#include "Engine.h"
#include "PhysicsTransform.h"

vsgPhysX::unique_ptr<physx::PxRigidActor> vsgPhysX::createActor(const physx::PxGeometry& geometry, physx::PxReal density, physx::PxMaterial* material)
{
    if (!material)
        material = vsgPhysX::Engine::instance()->defaultMaterial;
    unique_ptr<physx::PxRigidActor> uniquePtr;
    // do we want to accept a pose/shape offset?
    if (density > 0.0)
        uniquePtr.reset(physx::PxCreateDynamic(vsgPhysX::Engine::instance()->physics(), physx::PxTransform(physx::PxIdentity), geometry, *material, density));
    else
        uniquePtr.reset(physx::PxCreateStatic(vsgPhysX::Engine::instance()->physics(), physx::PxTransform(physx::PxIdentity), geometry, *material));
    return uniquePtr;
}

vsgPhysX::unique_ptr<physx::PxRigidActor> vsgPhysX::createBoxActor(const vsg::vec3& dimensions, physx::PxReal density, physx::PxMaterial* material)
{
    physx::PxBoxGeometry geometry{dimensions.x / 2.0f, dimensions.y / 2.0f, dimensions.z / 2.0f};
    return createActor(geometry, density, material);
}

vsgPhysX::unique_ptr<physx::PxRigidActor> vsgPhysX::createCapsuleActor(float height, float radius, physx::PxReal density, physx::PxMaterial* material)
{
    physx::PxCapsuleGeometry geometry{radius, height / 2};
    return createActor(geometry, density, material);
}

vsgPhysX::unique_ptr<physx::PxRigidActor> vsgPhysX::createPlaneActor(const vsg::vec4& plane, physx::PxMaterial* material)
{
    if (!material)
        material = vsgPhysX::Engine::instance()->defaultMaterial;
    vsg::quat orientation(vsg::vec3{1.0f, 0.0f, 0.0f}, plane.xyz);
    physx::PxTransform pose(0.0f, 0.0f, -plane.w, convert(orientation));
    // is it better for the pose to be a shape offset instead?
    return unique_ptr<physx::PxRigidActor>(physx::PxCreateStatic(vsgPhysX::Engine::instance()->physics(), pose, physx::PxPlaneGeometry(), *material));
}

vsgPhysX::unique_ptr<physx::PxRigidActor> vsgPhysX::createSphereActor(float radius, physx::PxReal density, physx::PxMaterial* material)
{
    physx::PxSphereGeometry geometry{radius};
    return createActor(geometry, density, material);
}

vsg::ref_ptr<vsg::Node> vsgPhysX::createNodeForActor(ActorBuilder& builder, vsgPhysX::unique_ptr<physx::PxRigidActor>&& actor)
{
    vsg::ref_ptr<PhysicsTransform> transform = PhysicsTransform::create(std::move(actor));

    std::vector<physx::PxShape*> shapes(transform->actor->getNbShapes());
    transform->actor->getShapes(shapes.data(), shapes.size());

    for (const physx::PxShape* shape : shapes)
    {
        vsg::GeometryInfo geomInfo;
        auto transformMatrix = convert(physx::PxMat44(shape->getLocalPose()));
        geomInfo.transform = transformMatrix;

        const physx::PxGeometry& geometry = shape->getGeometry();
        switch (geometry.getType())
        {
        // TODO: missing geometry types
        case physx::PxGeometryType::eSPHERE: {
            const auto& sphereGeom = static_cast<const physx::PxSphereGeometry&>(geometry);
            transform->addChild(builder.createSphere(sphereGeom, transformMatrix));
        }
        break;
        case physx::PxGeometryType::ePLANE: {
            const auto& planeGeom = static_cast<const physx::PxPlaneGeometry&>(geometry);
            transform->addChild(builder.createPlane(planeGeom, transformMatrix));
        }
        break;
        case physx::PxGeometryType::eCAPSULE: {
            const auto& capsuleGeom = static_cast<const physx::PxCapsuleGeometry&>(geometry);
            transform->addChild(builder.createCapsule(capsuleGeom, transformMatrix));
        }
        break;
        case physx::PxGeometryType::eBOX: {
            const auto& boxGeom = static_cast<const physx::PxBoxGeometry&>(geometry);
            transform->addChild(builder.createBox(boxGeom, transformMatrix));
        }
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
            // cones and cylinders are implemented like this for whatever reason
            break;
        }
    }

    return transform;
}
