#include "thread_local.h"
#include <gtest/gtest.h>
#include <thread>
#include <experimental/barrier>

TEST(ThreadLocal, SetAndGet) {
    ThreadLocal<int> tl;
    *tl = 42;
    ASSERT_EQ(*tl, 42);

    ThreadLocal<int> tl_other;
    *tl_other = 17;
    ASSERT_EQ(*tl_other, 17);

    ASSERT_EQ(*tl, 42);
}

TEST(ThreadLocal, AccessFromDifferentThreads) {
    ThreadLocal<std::string> tl;

    *tl = "first";
    ASSERT_EQ(*tl, "first");

    std::thread other_thread([&tl]() {
        *tl = "second";
        ASSERT_EQ(*tl, "second");
    });

    other_thread.join();

    ASSERT_EQ(*tl, "first");
}

TEST(ThreadLocal, AccessManyTimes) {
    ThreadLocal<int> tl;
    const size_t kAccesses = 100500;
    for (size_t i = 0; i <= kAccesses; ++i) {
        *tl = i;
    }
    ASSERT_EQ(*tl, kAccesses);
}

TEST(ThreadLocal, IterateThreadLocalValues) {
    ThreadLocal<int> tl;

    static const size_t kThreads = 10;
    twist::OnePassBarrier barrier{kThreads};

    auto accessor = [&tl, &barrier](int thread_index) {
        *tl = thread_index;

        barrier.PassThrough();

        // Writes to thread-local values ordered with the following reads
        // through barrier.PassThrough()

        size_t total = 0;
        size_t thread_count = 0;

        for (auto& value : tl) {
            ++thread_count;
            total += value;
        }

        ASSERT_EQ(thread_count, kThreads);
        ASSERT_EQ(total, kThreads * (kThreads - 1) / 2);
    };

    twist::ScopedExecutor executor;
    for (size_t i = 0; i < kThreads; ++i) {
        executor.Submit(accessor, i);
    }
}