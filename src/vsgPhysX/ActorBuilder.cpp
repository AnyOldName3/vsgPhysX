#include "ActorBuilder.h"

#include <geometry/PxBoxGeometry.h>
#include <geometry/PxCapsuleGeometry.h>
#include <geometry/PxSphereGeometry.h>

#include <vsg/maths/common.h>
#include <vsg/nodes/VertexIndexDraw.h>

#include <VsgVersionCheck.h>

vsg::ref_ptr<vsg::Node> vsgPhysX::ActorBuilder::createBox(const physx::PxBoxGeometry& boxGeom, const vsg::mat4& transform, const vsg::StateInfo& stateInfo)
{
    vsg::GeometryInfo geomInfo;
    geomInfo.transform = transform;
    geomInfo.dx = vsg::vec3(2 * boxGeom.halfExtents.x, 0.0f, 0.0f);
    geomInfo.dy = vsg::vec3(0.0f, 2 * boxGeom.halfExtents.y, 0.0f);
    geomInfo.dz = vsg::vec3(0.0f, 0.0f, 2 * boxGeom.halfExtents.z);
    return createBox(geomInfo, stateInfo);
}

vsg::ref_ptr<vsg::Node> vsgPhysX::ActorBuilder::createCapsule(const physx::PxCapsuleGeometry& capsuleGeom, const vsg::mat4& transform, const vsg::StateInfo& stateInfo)
{
    // This function is copied from VSG with minor modifications to support separate radius and height
#if VSG_API_VERSION_LESS(1, 1, 4)
    // Cache doesn't work in upstream vsg::Builder before this, and can't be made to work as StateInfo is not Comparable
    vsg::ref_ptr<vsg::Node> subgraph;
#else
    auto& subgraph = _physxCapsules[std::make_tuple(capsuleGeom.radius, capsuleGeom.halfHeight, transform, stateInfo)];
#endif
    if (subgraph)
    {
        return subgraph;
    }

    uint32_t instanceCount = 1;
    auto colors = vsg::vec4Array::create(instanceCount, vsg::vec4{1.0f, 1.0f, 1.0f, 1.0f});
    auto [t_origin, t_scale, t_top] = y_texcoord(stateInfo).value;

    auto dx = vsg::vec3(0.0f, capsuleGeom.radius, 0.0f);
    auto dy = vsg::vec3(0.0f, 0.0f, capsuleGeom.radius);
    auto dz = vsg::vec3(capsuleGeom.radius, 0.0f, 0.0f);

    auto bottom = vsg::vec3(-capsuleGeom.halfHeight, 0.0f, 0.0f);
    auto top = vsg::vec3(capsuleGeom.halfHeight, 0.0f, 0.0f);

    bool withEnds = true;

    unsigned int num_columns = 20;
    unsigned int num_rows = 6;

    unsigned int num_vertices = num_columns * 2;
    unsigned int num_indices = (num_columns - 1) * 6;

    if (withEnds)
    {
        num_vertices += num_columns * num_rows * 2;
        num_indices += (num_columns - 1) * (num_rows - 1) * 6 * 2;
    }

    auto vertices = vsg::vec3Array::create(num_vertices);
    auto normals = vsg::vec3Array::create(num_vertices);
    auto texcoords = vsg::vec2Array::create(num_vertices);
    auto indices = vsg::ushortArray::create(num_indices);

    vsg::vec3 v = dy;
    vsg::vec3 n = vsg::normalize(dy);
    vertices->set(0, bottom + v);
    normals->set(0, n);
    texcoords->set(0, vsg::vec2(0.0, t_origin));
    vertices->set(num_columns * 2 - 2, bottom + v);
    normals->set(num_columns * 2 - 2, n);
    texcoords->set(num_columns * 2 - 2, vsg::vec2(1.0, t_origin));

    vertices->set(1, top + v);
    normals->set(1, n);
    texcoords->set(1, vsg::vec2(0.0, t_top));
    vertices->set(num_columns * 2 - 1, top + v);
    normals->set(num_columns * 2 - 1, n);
    texcoords->set(num_columns * 2 - 1, vsg::vec2(1.0, t_top));

    for (unsigned int c = 1; c < num_columns - 1; ++c)
    {
        unsigned int vi = c * 2;
        float r = float(c) / float(num_columns - 1);
        float alpha = (r) * 2.0f * vsg::PIf;
        v = dx * (-sinf(alpha)) + dy * (cosf(alpha));
        n = normalize(v);

        vertices->set(vi, bottom + v);
        normals->set(vi, n);
        texcoords->set(vi, vsg::vec2(r, t_origin));

        vertices->set(vi + 1, top + v);
        normals->set(vi + 1, n);
        texcoords->set(vi + 1, vsg::vec2(r, t_top));
    }

    unsigned int i = 0;
    for (unsigned int c = 0; c < num_columns - 1; ++c)
    {
        unsigned lower = c * 2;
        unsigned upper = lower + 1;

        indices->set(i++, lower);
        indices->set(i++, lower + 2);
        indices->set(i++, upper);

        indices->set(i++, upper);
        indices->set(i++, lower + 2);
        indices->set(i++, upper + 2);
    }

    if (withEnds)
    {
        unsigned int base_vi = num_columns * 2;

        // bottom
        {
            for (unsigned int r = 0; r < num_rows; ++r)
            {
                float beta = ((float(r) / float(num_rows - 1)) - 1.0f) * vsg::PIf * 0.5f;
                float ty = t_origin + t_scale * float(r) / float(num_rows - 1);
                float cos_beta = cosf(beta);
                vsg::vec3 dz_sin_beta = dz * sinf(beta);

                v = dy * cos_beta + dz_sin_beta;
                n = normalize(v);

                unsigned int left_i = base_vi + r * num_columns;
                vertices->set(left_i, bottom + v);
                normals->set(left_i, n);
                texcoords->set(left_i, vsg::vec2(0.0f, ty));

                unsigned int right_i = left_i + num_columns - 1;
                vertices->set(right_i, bottom + v);
                normals->set(right_i, n);
                texcoords->set(right_i, vsg::vec2(1.0f, ty));

                for (unsigned int c = 1; c < num_columns - 1; ++c)
                {
                    unsigned int vi = left_i + c;
                    float alpha = (float(c) / float(num_columns - 1)) * 2.0f * vsg::PIf;
                    v = dx * (-sinf(alpha) * cos_beta) + dy * (cosf(alpha) * cos_beta) + dz_sin_beta;
                    n = normalize(v);
                    vertices->set(vi, bottom + v);
                    normals->set(vi, n);
                    texcoords->set(vi, vsg::vec2(float(c) / float(num_columns - 1), ty));
                }
            }

            for (unsigned int r = 0; r < num_rows - 1; ++r)
            {
                for (unsigned int c = 0; c < num_columns - 1; ++c)
                {
                    unsigned lower = base_vi + num_columns * r + c;
                    unsigned upper = lower + num_columns;

                    indices->set(i++, lower);
                    indices->set(i++, lower + 1);
                    indices->set(i++, upper);

                    indices->set(i++, upper);
                    indices->set(i++, lower + 1);
                    indices->set(i++, upper + 1);
                }
            }

            base_vi += num_columns * num_rows;
        }

        // top
        {
            for (unsigned int r = 0; r < num_rows; ++r)
            {
                float beta = ((float(r) / float(num_rows - 1))) * vsg::PIf * 0.5f;
                float ty = t_origin + t_scale * float(r) / float(num_rows - 1);
                float cos_beta = cosf(beta);
                vsg::vec3 dz_sin_beta = dz * sinf(beta);

                v = dy * cos_beta + dz_sin_beta;
                n = normalize(v);

                unsigned int left_i = base_vi + r * num_columns;
                vertices->set(left_i, top + v);
                normals->set(left_i, n);
                texcoords->set(left_i, vsg::vec2(0.0f, ty));

                unsigned int right_i = left_i + num_columns - 1;
                vertices->set(right_i, top + v);
                normals->set(right_i, n);
                texcoords->set(right_i, vsg::vec2(1.0f, ty));

                for (unsigned int c = 1; c < num_columns - 1; ++c)
                {
                    unsigned int vi = left_i + c;
                    float alpha = (float(c) / float(num_columns - 1)) * 2.0f * vsg::PIf;
                    v = dx * (-sinf(alpha) * cos_beta) + dy * (cosf(alpha) * cos_beta) + dz_sin_beta;
                    n = normalize(v);
                    vertices->set(vi, top + v);
                    normals->set(vi, n);
                    texcoords->set(vi, vsg::vec2(float(c) / float(num_columns - 1), ty));
                }
            }

            for (unsigned int r = 0; r < num_rows - 1; ++r)
            {
                for (unsigned int c = 0; c < num_columns - 1; ++c)
                {
                    unsigned lower = base_vi + num_columns * r + c;
                    unsigned upper = lower + num_columns;

                    indices->set(i++, lower);
                    indices->set(i++, lower + 1);
                    indices->set(i++, upper);

                    indices->set(i++, upper);
                    indices->set(i++, lower + 1);
                    indices->set(i++, upper + 1);
                }
            }
        }
    }

    if (transform != identity)
    {
        vsg::Builder::transform(transform, vertices, normals);
    }

    // setup geometry
    auto vid = vsg::VertexIndexDraw::create();

    vsg::DataList arrays;
    arrays.push_back(vertices);
    if (normals) arrays.push_back(normals);
    if (texcoords) arrays.push_back(texcoords);
    if (colors) arrays.push_back(colors);
    vid->assignArrays(arrays);

    vid->assignIndices(indices);
    vid->indexCount = static_cast<uint32_t>(indices->size());
    vid->instanceCount = instanceCount;

    subgraph = decorateAndCompileIfRequired({}, stateInfo, vid);
    return subgraph;
}

vsg::ref_ptr<vsg::Node> vsgPhysX::ActorBuilder::createPlane(const physx::PxPlaneGeometry& geometry, const vsg::mat4& transform, const vsg::StateInfo& stateInfo)
{
    // VSG's standard shader sets only support vec3 vertices, whereas we need vec4 ones to represent an infinite plane
    // Instead, just draw a big square
    vsg::GeometryInfo geomInfo;
    geomInfo.transform = transform;
    geomInfo.dx = vsg::vec3(0.0f, 10000.0f, 0.0f);
    geomInfo.dy = vsg::vec3(0.0f, 0.0f, 10000.0f);
    return createQuad(geomInfo, stateInfo);
}

vsg::ref_ptr<vsg::Node> vsgPhysX::ActorBuilder::createSphere(const physx::PxSphereGeometry& sphereGeom, const vsg::mat4& transform, const vsg::StateInfo& stateInfo)
{
    vsg::GeometryInfo geomInfo;
    geomInfo.transform = transform;
    geomInfo.dx = vsg::vec3(2 * sphereGeom.radius, 0.0f, 0.0f);
    geomInfo.dy = vsg::vec3(0.0f, 2 * sphereGeom.radius, 0.0f);
    geomInfo.dz = vsg::vec3(0.0f, 0.0f, 2 * sphereGeom.radius);
    return createSphere(geomInfo, stateInfo);
}
