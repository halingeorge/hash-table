#pragma once

#include "thread_local.h"
#include <atomic>

class RCULock {
public:
    void ReadLock() {
        __atomic_store_n(thread_local_.operator->(), version_.load() + kMaxThreadNumber, __ATOMIC_SEQ_CST);
    }

    void ReadUnlock() {
        __atomic_store_n(thread_local_.operator->(), 0, __ATOMIC_SEQ_CST);
    }

    void Synchronize() {
        for (const auto& value : thread_local_) {
            while (__atomic_load_n(&value, __ATOMIC_SEQ_CST) >= version_.load()) {
                std::this_thread::yield();
            }
        }
        version_.fetch_add(1);
    }

private:
    static const size_t kMaxThreadNumber = 1000;

    ThreadLocal<size_t> thread_local_;
    std::atomic<size_t> version_{1};
};
