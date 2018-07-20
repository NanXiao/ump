#include <atomic>
#include <deque>
#include <mutex>
#include <unordered_map>

#include "ump.h"

struct UMP::Impl
{
    std::mutex _pool_mu;
    std::unordered_map<size_t, std::deque<void*>> _slot_map;
    std::unordered_map<void*, size_t> _ptr_map;
    std::atomic_size_t _used_mem{};
    std::function<void*(size_t)> _alloc_fun;
    std::function<void(void*)> _free_fun;
};

UMP::UMP(std::function<void*(size_t)> alloc_fun, std::function<void(void*)> free_fun) :
        _impl(new Impl)
{
    _impl->_alloc_fun = alloc_fun;
    _impl->_free_fun = free_fun;
}

UMP::~UMP()
{
    for (auto& it : _impl->_slot_map)
    {
        auto& q = it.second;
        while (!q.empty())
        {
            auto v = q.front();
            q.pop_front();
            _impl->_free_fun(v);
        }
    }
}

void* UMP::allocate_memory(size_t size)
{
    if (!size)
    {
        return nullptr;
    }

    void *ptr = nullptr;
    std::unique_lock<std::mutex> pool_lock(_impl->_pool_mu);
    auto& q = _impl->_slot_map[size];
    if (q.empty())
    {
        ptr = _impl->_alloc_fun(size);
        if (ptr)
        {
            _impl->_ptr_map[ptr] = size;
            _impl->_used_mem += size;
        }
    }
    else
    {
        ptr = q.front();
        q.pop_front();
    }
    return ptr;
}

void UMP::free_memory(void *ptr)
{
    if (!ptr)
    {
        return;
    }

    std::unique_lock<std::mutex> pool_lock(_impl->_pool_mu);
    auto size = _impl->_ptr_map[ptr];
    _impl->_slot_map[size].push_front(ptr);
}

size_t UMP::get_used_memory()
{
    return _impl->_used_mem.load();
}
