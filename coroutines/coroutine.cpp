#include "coroutines/coroutine.hpp"

#include <iostream>

namespace crs
{

namespace detail
{

void coroutine_execution_context::context_function()
{
    try
    {
        fun_();
    }
    catch(...)
    {
        exception_ = std::current_exception();
    }
}


}
}

