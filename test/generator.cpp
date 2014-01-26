#include "coroutines/generator.hpp"

#include <gtest/gtest.h>


TEST(Generator, Iterator)
{
    crs::generator<int> it([](crs::generator_output<int> out){
        for(int i = 0; i < 10; i++)
        {
            *(out++) = i;
        }
    });

    int expected = 0;
    for(; it != crs::generator<int>(); ++it)
    {
        ASSERT_EQ(expected++, *it);
    }
    ASSERT_EQ(expected, 10);
}

TEST(Generator, EmptyIterator)
{
    crs::generator<int> it([](crs::generator_output<int>){});

    for(; it != crs::generator<int>(); ++it)
    {
        ASSERT_FALSE(true);
    }
}

/* Does not work now, generator is not a valid
TEST(Generator, Sequence)
{
    crs::generator<int> gen([](crs::generator_output<int> out){
        for(int i = 0; i < 10; i++)
        {
            *(out++) = i;
        }
    });

    int expected = 0;
    for(auto v : crs::to_sequence(gen))
    {
        ASSERT_EQ(expected++, v);
    }
    ASSERT_EQ(expected, 10);
}
*/

