find_package( Boost 1.54.0 COMPONENTS context system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_library(coroutines

	coroutine.cpp coroutine.hpp
	execution_context.cpp execution_context.hpp
	generator.hpp
)

target_link_libraries(coroutines

	${Boost_LIBRARIES}
)
