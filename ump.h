#pragma once

#include <functional>
#include <memory>

class UMP
{
private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
public:
    UMP(std::function<void*(size_t)> alloc_fun, std::function<void(void*)> free_fun);
    ~UMP();
    void *allocate_memory(size_t size);
    void free_memory(void *ptr);
    size_t get_used_memory();
};
