#pragma once

#include "coroutines/coroutine.hpp"

#include <boost/optional.hpp>
#include <vector>

// Various generators
namespace crs
{

// Generator output - to be used from within generator
template<typename T>
class generator_output
{
public:

    generator_output(boost::optional<T>& next)
        : next_(next)
    { }

    generator_output<T>& operator*()
    {
        return *this;
    }

    generator_output<T>& operator=(const T& v)
    {
        next_ = v;
        return *this;
    }

    generator_output<T>& operator=(T&& v)
    {
        next_ = std::move(v);
    }

    generator_output<T>& operator++()
    {
        advance();
        return *this;
    }

    generator_output<T>& operator++(int)
    {
        advance();
        return *this;
    }


private:

    void advance()
    {
        coroutine::yield();
    }

    boost::optional<T>& next_;
};

// Generator iterator - works as a new-style iterator
template<typename T>
class generator
{
public:

    // creates invalid generator
    generator()
    { }

    // creates valid generator
    template<typename Fn>
    generator(const Fn& f) // moving-out function will be possible in c++14
        : coroutine_([f, this](){ f(generator_output<T>(next_)); })
    {
        coroutine_.resume();
    }

    bool operator==(const generator<T>& o) const
    {
        return next_.is_initialized() == o.next_.is_initialized();
    }

    bool operator!=(const generator<T>& o) const
    {
        return next_.is_initialized() != o.next_.is_initialized();
    }

    const T& operator*() const
    {
        return *next_;
    }

    generator<T>& operator++()
    {
        advance();
        return *this;
    }

    // this is probably incorrect
    generator<T>& operator++(int)
    {
        advance();
        return *this;
    }

private:

    void advance()
    {
        next_.reset();
        if (coroutine_.is_valid() && !coroutine_.is_completed())
        {
            coroutine_.resume();
        }
    }

    coroutine coroutine_;
    boost::optional<T> next_;
};

// Wrapper for generator providing container-like interface
// TODO doesn't boost provide some adaptor from new-style iterators to containers
template<typename T>
class generator_sequence
{
public:

    typedef generator<T> iterator_type;
    typedef T value_type;

    generator_sequence(const generator<T>& gen)
        : gen_(gen)
    { }

    generator<T> begin() const { return gen_; }
    generator<T> end() const { return generator<T>(); }

private:

    generator<T> gen_;
};

// Function converting generator to sequence
template<typename T>
generator_sequence<T> to_sequence(const generator<T>& gen)
{
    return generator_sequence<T>(gen);
}

}

