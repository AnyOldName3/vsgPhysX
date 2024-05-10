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

    template<class T, class U>
    unique_ptr<T> unique_ptr_static_cast(unique_ptr<U>& rhs)
    {
        return unique_ptr<T>(static_cast<T*>(rhs.release()));
    }

    /* Currently disabled as PhysX defaults to RTTI off and I've not figured out how to test it
    template<class T, class U>
    unique_ptr<T> unique_ptr_dynamic_cast(unique_ptr<U>& rhs)
    {
        if (dynamic_cast<T*>(rhs.get()))
            return unique_ptr_static_cast<T>(rhs);
        else
            return nullptr;
    }*/
} // namespace vsgPhysX
