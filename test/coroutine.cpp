#include "coroutines/coroutine.hpp"

#include <gtest/gtest.h>

TEST(Corotuine, BasicTest)
{
    int checkpoint = 0;

    crs::coroutine coroutine([&checkpoint]() {
        checkpoint = 1;
        crs::execution_context::current_context()->yield();
        checkpoint = 2;
        crs::execution_context::current_context()->yield();
        checkpoint = 3;
    });

    ASSERT_EQ(checkpoint, 0);
    ASSERT_FALSE(coroutine.is_completed());
    coroutine.resume();
    ASSERT_EQ(checkpoint, 1);
    ASSERT_FALSE(coroutine.is_completed());
    coroutine.resume();
    ASSERT_EQ(checkpoint, 2);
    ASSERT_FALSE(coroutine.is_completed());
    coroutine.resume();
    ASSERT_EQ(checkpoint, 3);
    ASSERT_TRUE(coroutine.is_completed());
}


TEST(Corotuine, IsValid)
{
    crs::coroutine invalid;
    crs::coroutine valid([]() { });

    ASSERT_FALSE(invalid.is_valid());
    ASSERT_TRUE(valid.is_valid());
}

TEST(Corotuine, IsCompleted)
{
    crs::coroutine coroutine([](){});
    ASSERT_FALSE(coroutine.is_completed());
    coroutine.resume();
    ASSERT_TRUE(coroutine.is_completed());
}

TEST(Corotuine, Exceptions)
{
    crs::coroutine noex([](){});
    crs::coroutine ex([](){ throw std::runtime_error("test exception"); });

    ASSERT_FALSE(noex.get_exception());
    ASSERT_FALSE(ex.get_exception());

    noex.resume();
    ex.resume();

    ASSERT_FALSE(!!noex.get_exception());
    ASSERT_TRUE(!!ex.get_exception());

    try
    {
        std::rethrow_exception(ex.get_exception());
        ASSERT_TRUE(false); // should never be reached
    }
    catch(const std::exception& e)
    {
        ASSERT_EQ(std::string(e.what()), "test exception");
    }
}
