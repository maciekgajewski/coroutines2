#include "coroutines/execution_context.hpp"

#include <iostream>
#include <cassert>

namespace crs
{


static const unsigned DEFAULT_STACK_SIZE = 64*1024; // 64kb should be enough for anyone :)
static thread_local execution_context* current_execution_context = nullptr;


execution_context::execution_context()
    : stack_(new char[DEFAULT_STACK_SIZE])
{
    new_context_ = boost::context::make_fcontext(
                stack_.get() + DEFAULT_STACK_SIZE,
                DEFAULT_STACK_SIZE,
                &execution_context::static_context_function);
}

execution_context::~execution_context()
{
}

void execution_context::resume()
{
    if (!new_context_)
    {
        throw std::logic_error("coroutine already completed");
    }

    execution_context* previous = current_execution_context;
    current_execution_context = this;

    boost::context::jump_fcontext(&caller_context_, new_context_, reinterpret_cast<intptr_t>(this));

    current_execution_context = previous;

}

execution_context* execution_context::current_context()
{
    return current_execution_context;
}

void execution_context::yield()
{
    assert(current_execution_context == this);

    boost::context::jump_fcontext(new_context_, &caller_context_, 0);
}

void execution_context::static_context_function(intptr_t param)
{
    execution_context* _this = reinterpret_cast<execution_context*>(param);
    _this->context_function_wrapper();

}

void execution_context::context_function_wrapper()
{
    try
    {
        context_function();
    }
    catch(...)
    {
        std::cerr << "Uncought exception in coroutine" << std::endl;
        std::terminate();
    }
    auto temp = new_context_;
    new_context_ = nullptr;// to mark the completion
    boost::context::jump_fcontext(temp, &caller_context_, 0);
}


}
