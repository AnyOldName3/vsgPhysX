#pragma once

#include "Export.h"
#include "VsgVersionCheck.h"

namespace vsgPhysX
{
    namespace detail
    {
#if VSG_API_VERSION_LESS(1, 1, 4)
        template<typename T>
        using allocator = std::allocator<T>;
#else
        template<typename T>
        using allocator = vsg::allocator_affinity_physics<T>;
#endif

        class alignas(16) ScratchBlock
        {
            std::uint8_t buffer[16 * 1024];
        };
    } // namespace detail

    using ScratchBuffer = std::vector<detail::ScratchBlock, detail::allocator<detail::ScratchBlock>>;

    inline constexpr physx::PxU32 scratchBufferSize(const ScratchBuffer& buffer)
    {
        return static_cast<physx::PxU32>(std::min<std::size_t>(buffer.size() * sizeof(detail::ScratchBlock), std::numeric_limits<physx::PxU32>::max()));
    }
} // namespace vsgPhysX
