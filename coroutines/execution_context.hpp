#pragma once

#include <boost/context/fcontext.hpp>

#include <memory>

namespace crs
{

class execution_context
{
public:

    execution_context();
    virtual ~execution_context();

    void resume();
    bool is_completed() const { return new_context_ == nullptr; }

    static execution_context* current_context();

    /// MUST be called from another context
    void yield();

protected:

    // coroutine entry point, called only once, on the first call to 'resume'
    virtual void context_function() = 0;

private:

    std::unique_ptr<char[]> stack_;

    boost::context::fcontext_t caller_context_;
    boost::context::fcontext_t* new_context_ = nullptr;

    static void static_context_function(intptr_t param);
    void context_function_wrapper();
};

}

