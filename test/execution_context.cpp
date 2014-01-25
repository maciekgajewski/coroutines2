#include "coroutines/execution_context.hpp"

#include <gtest/gtest.h>

#include <functional>

struct simple_context : public crs::execution_context
{
    template<typename Fn>
    explicit simple_context(Fn&& fun)
    : fun_(std::move(fun))
    { }

    virtual void context_function() override
    {
        fun_();
    }

    std::function<void()> fun_;
};

TEST(ExecutionContext, BasicTest)
{
    int checkpoint = 0;

    simple_context context([&checkpoint]() {
        checkpoint = 1;
        crs::execution_context::current_context()->yield();
        checkpoint = 2;
        crs::execution_context::current_context()->yield();
        checkpoint = 3;
    });

    ASSERT_EQ(checkpoint, 0);
    ASSERT_FALSE(context.is_completed());
    context.resume();
    ASSERT_EQ(checkpoint, 1);
    ASSERT_FALSE(context.is_completed());
    context.resume();
    ASSERT_EQ(checkpoint, 2);
    ASSERT_FALSE(context.is_completed());
    context.resume();
    ASSERT_EQ(checkpoint, 3);
    ASSERT_TRUE(context.is_completed());
}
