#pragma once

/* <editor-fold desc="MIT License">

Copyright(c) 2018-2024 Robert Osfield, Chris Djali

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <type_traits>

namespace vsgPhysX
{
    namespace details
    {
        template<typename, typename = void>
        constexpr bool has_acquireReference_release_members = false;

        template<typename T>
        constexpr bool has_acquireReference_release_members<T, std::void_t<decltype(std::declval<T>().acquireReference()), decltype(std::declval<T>().release())>> = true;
    } // namespace details

    /// smart pointer that works with objects that have intrusive reference counting, mirroring vsg::ref_ptr
    /// unfortunately, not all PhysX classes supporting this share a common base class, so we have to define a type trait
    /// we check the type trait in the destructor as that ensures this works nicely with forward declarations
    template<class T>
    class ref_ptr
    {
    public:
        using element_type = T;

        template<class R>
        static ref_ptr createNoAcquireRef(R* ptr) noexcept
        {
            ref_ptr result = ref_ptr(nullptr);
            result._ptr = ptr;
            return result;
        }

        ref_ptr() noexcept :
            _ptr(nullptr) {}

        ref_ptr(const ref_ptr& rhs) noexcept :
            _ptr(rhs._ptr)
        {
            if (_ptr) _ptr->acquireReference();
        }

        /// move constructor
        template<class R>
        ref_ptr(ref_ptr<R>&& rhs) noexcept :
            _ptr(rhs._ptr)
        {
            rhs._ptr = nullptr;
        }

        template<class R>
        ref_ptr(const ref_ptr<R>& ptr) noexcept :
            _ptr(ptr._ptr)
        {
            if (_ptr) _ptr->acquireReference();
        }

        explicit ref_ptr(T* ptr) noexcept :
            _ptr(ptr)
        {
            if (_ptr) _ptr->acquireReference();
        }

        template<class R>
        explicit ref_ptr(R* ptr) noexcept :
            _ptr(ptr)
        {
            if (_ptr) _ptr->acquireReference();
        }

        // std::nullptr_t requires extra header
        ref_ptr(decltype(nullptr)) noexcept :
            ref_ptr() {}

        ~ref_ptr()
        {
            static_assert(details::has_acquireReference_release_members<T>, "Must have acquireReference and release");
            if (_ptr) _ptr->release();
        }

        void reset()
        {
            if (_ptr) _ptr->release();
            _ptr = nullptr;
        }

        ref_ptr& operator=(T* ptr)
        {
            if (ptr == _ptr) return *this;

            T* temp_ptr = _ptr;

            _ptr = ptr;

            if (_ptr) _ptr->acquireReference();

            // unref the original pointer after ref in case the old pointer object is a parent of the new pointer's object
            if (temp_ptr) temp_ptr->release();

            return *this;
        }

        ref_ptr& operator=(const ref_ptr& rhs)
        {
            if (rhs._ptr == _ptr) return *this;

            T* temp_ptr = _ptr;

            _ptr = rhs._ptr;

            if (_ptr) _ptr->acquireReference();

            // unref the original pointer after ref in case the old pointer object is a parent of the new pointer's object
            if (temp_ptr) temp_ptr->release();

            return *this;
        }

        template<class R>
        ref_ptr& operator=(const ref_ptr<R>& rhs)
        {
            if (rhs._ptr == _ptr) return *this;

            T* temp_ptr = _ptr;

            _ptr = rhs._ptr;

            if (_ptr) _ptr->acquireReference();

            // unref the original pointer after ref in case the old pointer object is a parent of the new pointer's object
            if (temp_ptr) temp_ptr->release();

            return *this;
        }

        /// move assignment
        template<class R>
        ref_ptr& operator=(ref_ptr<R>&& rhs)
        {
            if (rhs._ptr == _ptr) return *this;

            if (_ptr) _ptr->release();

            _ptr = rhs._ptr;

            rhs._ptr = nullptr;

            return *this;
        }

        template<class R>
        ref_ptr& assignNoAcquireRef(R* rhs)
        {
            if (rhs == _ptr) return *this;

            if (_ptr) _ptr.release();

            _ptr = rhs;

            return *this;
        }

        template<class R>
        bool operator<(const ref_ptr<R>& rhs) const { return (_ptr < rhs._ptr); }

        template<class R>
        bool operator==(const ref_ptr<R>& rhs) const { return (rhs._ptr == _ptr); }

        template<class R>
        bool operator!=(const ref_ptr<R>& rhs) const { return (rhs._ptr != _ptr); }

        template<class R>
        bool operator<(const R* rhs) const { return (_ptr < rhs); }

        template<class R>
        bool operator==(const R* rhs) const { return (rhs == _ptr); }

        template<class R>
        bool operator!=(const R* rhs) const { return (rhs != _ptr); }

        bool valid() const noexcept { return _ptr != nullptr; }

        explicit operator bool() const noexcept { return valid(); }

        // potentially dangerous automatic type conversion, could cause dangling pointer if ref_ptr<> assigned to C pointer and ref_ptr<> destruction causes an object delete.
        operator T*() const noexcept { return _ptr; }

        void operator[](int) const = delete;

        T& operator*() const noexcept { return *_ptr; }

        T* operator->() const noexcept { return _ptr; }

        T* get() const noexcept { return _ptr; }

        void swap(ref_ptr& rhs) noexcept
        {
            T* temp_ptr = _ptr;
            _ptr = rhs._ptr;
            rhs._ptr = temp_ptr;
        }

        template<class R>
        ref_ptr<R> cast() const { return ref_ptr<R>(_ptr ? _ptr->template cast<R>() : nullptr); }

    protected:
        template<class R>
        friend class ref_ptr;

        T* _ptr;
    };

} // namespace vsgPhysX
