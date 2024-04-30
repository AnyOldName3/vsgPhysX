#pragma once

#include <memory>

namespace vsgPhysX
{
    template<class T>
    struct pxRelease
    {
        void operator()(T* object)
        {
            if (object)
                object->release();
        }
    };

    template<class T>
    using unique_ptr = std::unique_ptr<T, pxRelease<T>>;
} // namespace vsgPhysX
