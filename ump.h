#pragma once

#include <atomic>
#include <deque>
#include <functional>
#include <mutex>
#include <unordered_map>

class UMP
{
private:
    std::mutex _pool_mu;
    std::unordered_map<size_t, std::deque<void*>> _slot_map;
    std::unordered_map<void*, size_t> _ptr_map;
    std::atomic_size_t _used_mem{};
    std::function<void*(size_t)> _alloc_fun;
    std::function<void(void*)> _free_fun;
public:
    UMP(std::function<void*(size_t)> alloc_fun, std::function<void(void*)> free_fun) :
            _alloc_fun(alloc_fun), _free_fun(free_fun)
    {
    }

    ~UMP()
    {
        for (auto& it : _slot_map)
        {
            auto& q = it.second;
            while (!q.empty())
            {
                auto v = q.front();
                q.pop_front();
                _free_fun(v);
            }
        }
    }

    void *allocate_memory(size_t size)
    {
        if (!size)
        {
            return nullptr;
        }

        void *ptr = nullptr;
        std::unique_lock<std::mutex> pool_lock(_pool_mu);
        auto& q = _slot_map[size];
        if (q.empty())
        {
            ptr = _alloc_fun(size);
            _ptr_map[ptr] = size;
            _used_mem += size;
        }
        else
        {
            ptr = q.front();
            q.pop_front();
        }
        return ptr;
    }

    void free_memory(void *ptr)
    {
        if (!ptr)
        {
            return;
        }

        std::unique_lock<std::mutex> pool_lock(_pool_mu);
        auto size = _ptr_map[ptr];
        _slot_map[size].push_front(ptr);
    }

    size_t get_used_memory()
    {
        return _used_mem.load();
    }
};
