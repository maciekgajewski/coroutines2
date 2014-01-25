#pragma once

#include <memory>
#include <exception>
#include <functional>
#include <cassert>

#include "coroutines/execution_context.hpp"

namespace crs
{

namespace detail
{

struct coroutine_execution_context : public execution_context, public std::enable_shared_from_this<coroutine_execution_context>
{
public:

    template<typename Fn>
    explicit coroutine_execution_context(Fn&& fun)
    : fun_(std::move(fun))
    { }

    virtual void context_function();

    std::exception_ptr get_exception() const
    {
        return exception_;
    }

private:

    std::function<void()> fun_;
    std::exception_ptr exception_;
};

}


// Main coroutine class
class coroutine
{
public:

    /// Creates invalid coroutine
    coroutine();

    /// Creates valid, executable coroutine
    template<typename Fn>
    explicit coroutine(Fn&& fn)
    : context_(std::make_shared<detail::coroutine_execution_context>(std::move(fn)))
    { }

    /// Creates shallow copy, sharing the internal state with the original
    coroutine(const coroutine&) = default;


    /// Test coroutine validity
    bool is_valid() const
    {
        return !!context_;
    }

    /// Can be called only from outside of valid coroutine.
    /// Calling on invalid or completed coroutine results in undefined behaviour
    void resume()
    {
        context_->resume();
    }

    /// Returns false is coroutine is not completed, and further calls to 'resume' are required.
    /// Calling on invalid coroutine results in undefined behaviour
    bool is_completed() const
    {
        return context_->is_completed();
    }

    /// If coroutine ended with uncought exception, this is the exception.
    std::exception_ptr get_exception()
    {
        return context_->get_exception();
    }

    /// Suspends execution of current corotuine. Must be called from within valid corotuine
    static void yield()
    {
        execution_context* cc = execution_context::current_context();
        assert(cc);
        cc->yield();
    }

    /// If called from withing runnig coroutine, returns valid copy of the coroutine
    // TODO what if called from the outside? crahs ot return invalid coro?
    static coroutine current_coroutine()
    {
        execution_context* cc = execution_context::current_context();
        assert(cc);
        detail::coroutine_execution_context* cec = static_cast<detail::coroutine_execution_context*>(cc);
        return coroutine(cec->shared_from_this(), 1);
    }

private:

    typedef std::shared_ptr<detail::coroutine_execution_context> context_ptr;

    coroutine(const context_ptr& ctx, int /*dummy*/)
    : context_(ctx)
    { }

    context_ptr context_;


};

}
