#pragma once

#include <new>
#include "memory/memory.hpp"

/**
 * override global operator new.
 * https://en.cppreference.com/w/cpp/memory/new/operator_new
 */
NODISCARD void* operator new(size_t size) 
{
    if (void* ptr = Engine::Memory::Malloc(size))
    {
        return ptr;
    }
    throw std::bad_alloc{};
}

NODISCARD void* operator new(size_t size, const std::nothrow_t&) noexcept
{
    return Engine::Memory::Malloc(size);
}

NODISCARD void* operator new[](size_t size)
{ 
    if (void* ptr = Engine::Memory::Malloc(size))
    {
        return ptr;
    }
    throw std::bad_alloc{};
}

NODISCARD void* operator new[](size_t size, const std::nothrow_t&) noexcept
{
    return Engine::Memory::Malloc(size);
}

/** override global operator delete */
void operator delete(void* ptr) 
{ 
    Engine::Memory::Free(ptr);
}

void operator delete(void* ptr, const std::nothrow_t&)
{
    Engine::Memory::Free(ptr);
}

void operator delete[](void* ptr) 
{ 
    Engine::Memory::Free(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t&)
{
    Engine::Memory::Free(ptr);
}

void operator delete (void* ptr, size_t size) { Engine::Memory::Free(ptr); }
void operator delete[](void* ptr, size_t size) {Engine::Memory::Free(ptr); }
void operator delete (void* ptr, size_t size, const std::nothrow_t&) noexcept { Engine::Memory::Free(ptr); }
void operator delete[](void* ptr, size_t size, const std::nothrow_t&) noexcept {Engine::Memory::Free(ptr); }
