#include "fluffytest.h"

#include <fluffycoin/utils/AnyMap.h>

#include <atomic>

using namespace fluffycoin;

TEST(AnyMap, NoCopy)
{
    AnyMap map;

    struct Z
    {
        static std::atomic<int> &ref()
        {
            static std::atomic<int> ref;
            return ref;
        }
        Z() { ref()++; }
        Z(Z &&r) { (void)r; }
        Z(const Z &r) { (void)r; ref()++; }
        Z &operator=(Z &&r) { (void)r; ref()--; return *this; }
        Z &operator=(const Z &r) { (void)r; ref()++; return *this; }

        ~Z()
        {
            EXPECT_EQ(ref(), 1);
        }
    };

    {
        Z &zapata = map.get<Z>();
        (void)zapata;
    }

    AnyMap clone(std::move(map));
}

TEST(AnyMap, DataStays)
{
    AnyMap map;

    struct Data
    {
        int i = 0;
    };

    // Set lvalue
    {
        Data d;
        d.i = 123;
        map.set(d);
    }
    {
        EXPECT_EQ(map.get<Data>().i, 123);
    }

    // Get
    {
        map.get<Data>().i = 234;
    }
    {
        EXPECT_EQ(map.get<Data>().i, 234);
    }

    // Set rvalue
    {
        Data d;
        d.i = 456;
        map.set(std::move(d));
    }
    {
        EXPECT_EQ(map.get<Data>().i, 456);
    }
}

TEST(AnyMap, RvalueConstruct)
{
    struct R
    {
        int i = 555;
        static std::atomic<int> &ref()
        {
            static std::atomic<int> ref;
            return ref;
        }
        R() { ref()++; }
        R(R &&r) { (void)r; }
        R(const R &r) { (void)r; ref()++; }
        R &operator=(R &&r) { (void)r; ref()--; return *this; }
        R &operator=(const R &r) { (void)r; ref()++; return *this; }

        ~R()
        {
            EXPECT_EQ(ref(), 1);
        }
    };
    EXPECT_EQ(AnyMap().get<R>().i, 555);
    EXPECT_EQ(R::ref(), 1);
}

TEST(AnyMap, UniquePointer)
{
    AnyMap map;
    std::unique_ptr<int> pt = std::make_unique<int>(222);
    map.set(std::move(pt));

    EXPECT_EQ(map.get<int>(), 222);
}

TEST(AnyMap, NotCopyable)
{
    AnyMap map;

    struct NoCopy
    {
        int i = 0;
        NoCopy() = default;
        NoCopy(NoCopy &&) = delete;
        NoCopy(const NoCopy &) = delete;
        NoCopy &operator=(NoCopy &&) = delete;
        NoCopy &operator=(const NoCopy &) = delete;
        ~NoCopy() = default;
    };

    std::unique_ptr<NoCopy> pt = std::make_unique<NoCopy>();
    pt->i = 789;
    map.set(std::move(pt));

    EXPECT_EQ(map.get<NoCopy>().i, 789);

    AnyMap copy(map);
    EXPECT_EQ(copy.get<NoCopy>().i, 0);
    EXPECT_EQ(AnyMap().get<NoCopy>().i, 0);

    AnyMap memory(std::move(map));
    EXPECT_EQ(memory.get<NoCopy>().i, 789);
}

TEST(AnyMap, CopyAssignment)
{
    AnyMap map;
    map.get<int>() = 123;
    AnyMap copy;
    copy.get<int>() = 222;
    copy = map;
    EXPECT_EQ(copy.get<int>(), 123);
}
