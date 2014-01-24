#pragma once

#include <boost/context/fcontext.hpp>

#include <memory>
#include <functional>

namespace crs
{

class execution_context : public std::enable_shared_from_this<execution_context>
{
public:

    execution_context(const std::function<void()>& fun, const std::string& name = std::string("unnamed"));
    ~execution_context();

    void resume();
    bool is_completed() const { return new_context_ == nullptr; }

    static execution_context* current_context();

    // MUST be called from inside coroutine
    void yield();

private:

    std::function<void()> function_;
    std::unique_ptr<char[]> stack_;
    std::string name_;

    boost::context::fcontext_t caller_context_;
    boost::context::fcontext_t* new_context_ = nullptr;

    static void static_context_function(intptr_t param);
    void context_function();

};

}

