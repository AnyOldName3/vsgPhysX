#include "Actor.h"

#include <PxRigidDynamic.h>
#include <PxRigidStatic.h>
#include <extensions/PxSimpleFactory.h>
#include <geometry/PxPlaneGeometry.h>

#include <vsg/utils/Builder.h>

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

vsgPhysX::unique_ptr<physx::PxRigidActor> vsgPhysX::createPlaneActor(const vsg::vec4& plane, physx::PxMaterial* material)
{
    if (!material)
        material = vsgPhysX::Engine::instance()->defaultMaterial;
    vsg::quat orientation(vsg::vec3{1.0f, 0.0f, 0.0f}, plane.xyz);
    physx::PxTransform pose(0.0f, 0.0f, -plane.w, convert(orientation));
    // is it better for the pose to be a shape offset instead?
    return unique_ptr<physx::PxRigidActor>(physx::PxCreateStatic(vsgPhysX::Engine::instance()->physics(), pose, physx::PxPlaneGeometry(), *material));
}

vsg::ref_ptr<vsg::Node> vsgPhysX::createNodeForActor(vsg::Builder& builder, vsgPhysX::unique_ptr<physx::PxRigidActor>&& actor)
{
    vsg::ref_ptr<PhysicsTransform> transform = PhysicsTransform::create(std::move(actor));

    std::vector<physx::PxShape*> shapes(transform->actor->getNbShapes());
    transform->actor->getShapes(shapes.data(), shapes.size());

    for (const physx::PxShape* shape : shapes)
    {
        vsg::GeometryInfo geomInfo;
        geomInfo.transform = convert(physx::PxMat44(shape->getLocalPose()));

        const physx::PxGeometry& geometry = shape->getGeometry();
        switch (geometry.getType())
        {
        // TODO: missing geometry types
        case physx::PxGeometryType::eSPHERE:
            break;
        case physx::PxGeometryType::ePLANE:
            break;
        case physx::PxGeometryType::eCAPSULE:
            break;
        case physx::PxGeometryType::eBOX: {
            const auto& boxGeom = static_cast<const physx::PxBoxGeometry&>(geometry);
            geomInfo.dx = vsg::vec3(2 * boxGeom.halfExtents.x, 0.0f, 0.0f);
            geomInfo.dy = vsg::vec3(0.0f, 2 * boxGeom.halfExtents.y, 0.0f);
            geomInfo.dz = vsg::vec3(0.0f, 0.0f, 2 * boxGeom.halfExtents.z);
            transform->addChild(builder.createBox(geomInfo));
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
            break;
        }
    }

    return transform;
}
