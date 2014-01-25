#include "coroutines/generator.hpp"

#include <gtest/gtest.h>


TEST(Generator, Iterator)
{
    crs::generator_iterator<int> it([](crs::generator_output<int> out){
        for(int i = 0; i < 10; i++)
        {
            *(out++) = i;
        }
    });

    int expected = 0;
    for(; it != crs::generator_iterator<int>(); it++)
    {
        ASSERT_EQ(expected++, *it);
    }
}

TEST(Generator, EmptyIterator)
{
    crs::generator_iterator<int> it([](crs::generator_output<int>){});

    for(; it != crs::generator_iterator<int>(); it++)
    {
        ASSERT_FALSE(true);
    }
}
