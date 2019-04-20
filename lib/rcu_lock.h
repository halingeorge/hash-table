#pragma once

#include "thread_local.hpp"

#include <twist/stdlike/atomic.hpp>
#include <twist/threading/spin_wait.hpp>

#include <iostream>

namespace solutions {

class RCULock {
public:
    void ReadLock() {
        __atomic_store_n(thread_local_.operator->(), version_.load() + 1000, __ATOMIC_SEQ_CST);
    }

    void ReadUnlock() {
        __atomic_store_n(thread_local_.operator->(), 0, __ATOMIC_SEQ_CST);
    }

    void Synchronize() {
        for (const auto& value : thread_local_) {
            while (__atomic_load_n(&value, __ATOMIC_SEQ_CST) >= version_.load()) {
                twist::th::this_thread::yield();
            }
        }
        version_.fetch_add(1);
    }

    void lock_shared() {  // NOLINT
        ReadLock();
    }

    void unlock_shared() {  // NOLINT
        ReadUnlock();
    }

private:
    ThreadLocal<size_t> thread_local_;
    twist::atomic<size_t> version_{1};
};

}  // namespace solutions