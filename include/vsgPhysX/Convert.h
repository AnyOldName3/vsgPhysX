#pragma once

#include <foundation/PxMat44.h>
#include <foundation/PxQuat.h>
#include <foundation/PxVec3.h>
#include <foundation/PxVec4.h>

#include <vsg/maths/mat4.h>
#include <vsg/maths/quat.h>
#include <vsg/maths/vec3.h>
#include <vsg/maths/vec4.h>

namespace vsgPhysX
{
    template<typename T>
    constexpr vsg::t_vec3<T> convert(const physx::PxVec3T<T>& px)
    {
        return vsg::t_vec3<T>(px.x, px.y, px.z);
    }

    template<typename T>
    constexpr physx::PxVec3T<T> convert(const vsg::t_vec3<T>& vsg)
    {
        return physx::PxVec3T<T>(vsg.x, vsg.y, vsg.z);
    }

    template<typename T>
    constexpr vsg::t_vec4<T> convert(const physx::PxVec4T<T>& px)
    {
        return vsg::t_vec4<T>(px.x, px.y, px.z, px.w);
    }

    template<typename T>
    constexpr physx::PxVec4T<T> convert(const vsg::t_vec4<T>& vsg)
    {
        return physx::PxVec4T<T>(vsg.x, vsg.y, vsg.z, vsg.w);
    }

    template<typename T>
    constexpr vsg::t_quat<T> convert(const physx::PxQuatT<T>& px)
    {
        return vsg::t_quat<T>(px.x, px.y, px.z, px.w);
    }

    template<typename T>
    constexpr physx::PxQuatT<T> convert(const vsg::t_quat<T>& vsg)
    {
        return physx::PxQuatT<T>(vsg.x, vsg.y, vsg.z, vsg.w);
    }

    template<typename T>
    constexpr vsg::t_mat4<T> convert(const physx::PxMat44T<T>& px)
    {
        return vsg::t_mat4<T>(convert(px.column0), convert(px.column1), convert(px.column2), convert(px.column3));
    }

    template<typename T>
    constexpr physx::PxMat44T<T> convert(const vsg::t_mat4<T>& vsg)
    {
        return physx::PxMat44T<T>(convert(vsg.value[0]), convert(vsg.value[1]), convert(vsg.value[2]), convert(vsg.value[3]));
    }
} // namespace vsgPhysX
