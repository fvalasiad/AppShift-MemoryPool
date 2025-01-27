/**
 * AppShift Memory Pool v2.0.0 Allocator for C++ containers.
 *
 * Copyright 2020-present Sapir Shemer, DevShift (devshift.biz)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * @author Valasiadis Fotios
 */
#pragma once

#include <memory>
#include "MemoryPool.h"

namespace AppShift::Memory {

template<class T>
class MemoryPoolAllocator {
    MemoryPool * mp;
    size_t block_size;

    template<class U>
    friend class MemoryPoolAllocator;

    template<class A, class B>
    friend bool operator==(const MemoryPoolAllocator<A>&, const MemoryPoolAllocator<B>&);
    
public:
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using const_pointer = const T*;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;
    
    template<class Type>
    struct rebind {
        using other = MemoryPoolAllocator<Type>;
    };

    MemoryPoolAllocator(size_type block_size = MEMORYPOOL_DEFAULT_BLOCK_SIZE) {
        mp = new MemoryPool(block_size);
        this->block_size = block_size;
    }

    template<class U>
    MemoryPoolAllocator(const MemoryPoolAllocator<U>& alloc) {
        mp = new MemoryPool(alloc.block_size);
        this->block_size = alloc.block_size;
    }

    MemoryPoolAllocator(const MemoryPoolAllocator& alloc) {
        mp = new MemoryPool(alloc.block_size);
        this->block_size = alloc.block_size;
    }

    template<class U>
    MemoryPoolAllocator(MemoryPoolAllocator<U>&& alloc) noexcept {
        mp = alloc.mp;
        this->block_size = alloc.block_size;

        alloc.mp = nullptr;
        alloc.block_size = 0;
    }

    MemoryPoolAllocator(MemoryPoolAllocator&& alloc) noexcept {
        mp = alloc.mp;
        this->block_size = alloc.block_size;

        alloc.mp = nullptr;
        alloc.block_size = 0;
    }

    template<class U>
    MemoryPoolAllocator& operator=(const MemoryPoolAllocator<U>& alloc) {
        if(this->block_size != alloc.block_size) {
            delete mp;

            mp = new MemoryPool(alloc.block_size);
            this->block_size = alloc.block_size;
        }
        return *this;
    }

    MemoryPoolAllocator& operator=(const MemoryPoolAllocator& alloc) {
        if(this->block_size != alloc.block_size) {
            delete mp;

            mp = new MemoryPool(alloc.block_size);
            this->block_size = alloc.block_size;
        }
        return *this;
    }

    template<class U>
    MemoryPoolAllocator& operator=(MemoryPoolAllocator<U>&& alloc) noexcept {
        if(this->block_size != alloc.block_size) {
            delete mp;

            mp = alloc.mp;
            this->block_size = alloc.block_size;

            alloc.mp = nullptr;
            alloc.block_size = 0;
        }
        return *this;
    }

    MemoryPoolAllocator& operator=(MemoryPoolAllocator&& alloc) noexcept {
        if(this->block_size != alloc.block_size) {
            delete mp;

            mp = alloc.mp;
            this->block_size = alloc.block_size;

            alloc.mp = nullptr;
            alloc.block_size = 0;
        }
        return *this;
    }
    
    ~MemoryPoolAllocator() {
        delete mp;
    }

    pointer address(reference x) const noexcept {
        return &x;
    }

    const_pointer address(const_reference x) const noexcept {
        return &x;
    }

    pointer allocate(size_type n, const void * hint = 0) {
        return (T*) mp->allocate(n * sizeof(T));
    }

    void deallocate(pointer p, size_type n) {
        mp->free(p);
    }

    size_type max_size() const noexcept {
        return block_size / sizeof(T);
    }

    template<class U, class... Args>
    void construct(U* p, Args&&... args) {
        new ((void*) p) U (std::forward<Args>(args)...);
    }

    template<class U>
    void destroy(U* p) {
        p->~U();
    }
};

template<class A, class B>
inline bool operator==(const MemoryPoolAllocator<A>& a, const MemoryPoolAllocator<B>& b) {
    return a.mp == b.mp;
}

template<class A, class B>
inline bool operator!=(const MemoryPoolAllocator<A>& a, const MemoryPoolAllocator<B>& b) {
    return !(a == b);
}

}